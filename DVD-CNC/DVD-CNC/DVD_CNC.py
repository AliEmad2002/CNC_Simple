import msvcrt                                   # for 'getch()' for manual control
import time                                     # for 'sleep()'
import serial                                   # for communicating with controller
import matplotlib.pyplot as plt                 # for plotting graphical simualtion & accessing png file on the hard disk
import math                                     # for triagnomic functions.
from xml.dom import minidom                     # for accessing parsing svg files

def eval_newton_poly(X, Y, V) :
    M = len(X)
    D = []
    print(X)
    print(Y)
    #print(V)
    for i in range(M) :
        D.append([Y[i]])
        for j in range(M-1) :
            D[i].append(0)
    for j in range (1, M-1) :
        for k in range (j, M) :
            D[k][j] = (D[k][j-1] - D[k-1][j-1]) / (X[k] - X[k-j]) ;
    A = []
    for j in range(M) :
        A.append(D[j][j])
    S = []
    for _ in range(M) :
        S.append(0)
    S[M-1] = A[M-1]
    P = []
    for v in V :
        for j in range (M-2, -1, -1) :
            S[j] = A[j] + (v-X[j]) * S[j+1]
        P.append(S[0])
        if abs(S[0])>1000 :
            print('##########################################3')
            print(X)
            print(Y)
            print(v)
            print('##########################################3')
    return P

def element_unbalances_list(element, lst) :
    if len(lst) < 2 :
        return False
    if lst[-1] > lst[-2] :
        if element > lst[-1] :
            return False
        else :
            return True
    if lst[-1] < lst[-2] :
        if element < lst[-1] :
            return False
        else :
            return True

def factorial(x) :
    factorial = 1
    for i in range(2,x+1) :
        factorial *= i
    return factorial

def binomial_coefficient(n,k) :
    if (n==k) :
        return 1
    cof = k+1
    for i in range(k+2,n+1) :
        cof *= i
    cof /= factorial(n-k)
    return cof

def find_Bezier_points(X, Y, m) :
    n = len(X)-1
    dt = 1 / m
    t = 0
    points = []
    for _ in range(m+1) :
        B = [0, 0]
        for i in range(n+1) :
            d = ((1-t)**(n-i))
            if t==1 and n==i:
                d=1
            b = binomial_coefficient(n,i)
            factor = b * d * (t**i)
            B[0] += factor * X[i]
            B[1] += factor * Y[i]
        points.append(B)
        t += dt
    return points


class plotter :
    def __init__(self, serial_port, baudrate, X_axis_length, Y_axis_length, head_interval=0.25) :
        self.serial_port = serial_port
        self.baudrate = baudrate
        self.X_axis_length = X_axis_length
        self.Y_axis_length = Y_axis_length
        self.head_interval = head_interval # sec.
        self.controller_is_connected = False
        self.current_pix = [0, 0]
        self.undrawn_pixels = []
        self.len_y = 0
        self.len_x = 0
        self.factor = []
        self.delta = 0.1
        self.displacement_vector = [0, 0]

    def connect_controller(self) :
        if self.controller_is_connected == False :
            self.controller = serial.Serial(port=self.serial_port, baudrate=self.baudrate, timeout=100)
            time.sleep(2) # wait for controller to connect
            self.controller_is_connected = True
            print('microcontroller connected.')

    def manual_movement(self) :
        self.connect_controller()
        cmd = ''
        x_steps = 0
        y_steps = 0
        while 1 :
            direction = chr(msvcrt.getch()[0])
            if direction == '\xe0' : #key for middle keyboard keys, i.e.: arrows, insert, home, pgup, del, end and pgdn
                direction = chr(msvcrt.getch()[0])
            elif direction == '\x1b' : # esc key
                break
            elif direction == '0' :
                cmd = '1' #zero here!
            else :
                print('wrong input!')
            if direction == 'H' : # upper arrow
                cmd = 'Y'
                y_steps += 1
            elif direction == 'P' : # lower arrow
                cmd = 'y'
                y_steps -= 1
            elif direction == 'M' : # right arrow
                cmd = 'X'
                x_steps += 1
            elif direction == 'K' : # left arrow
                cmd = 'x'
                x_steps -= 1
            elif direction == 'I' : # page up
                cmd = 'Z'
            elif direction == 'Q' : # page down
                cmd = 'z'
            elif direction == 'O' : # end
                cmd = '2' # max here!
            elif direction == 'G' : # home
                cmd = '3' # soft zero
                x_steps = 0
                y_steps = 0
            self.controller.write(bytes(cmd, 'utf-8'))
            # wait for controller's response:
            self.controller.inWaiting()
            response = chr(self.controller.read()[0])
            
            print('sent: ', cmd, ', recived: ', response, ', [x,y]=',[x_steps,y_steps], end='          \r')
            if response != cmd :
                print('\nBad controller response!', end='                         \r')
        print(50*' ')

    def find_phase_angle_with(self, dist_disp_vect) :
        if self.displacement_vector == [0, 0] : # if the machine is in stationary mode, feel free to start going any where
            return 0
        dot_product = self.displacement_vector[0]*dist_disp_vect[0] + self.displacement_vector[1]*dist_disp_vect[1]
        a_mag_squared = self.displacement_vector[0]**2 + self.displacement_vector[1]**2
        b_mag_squared = dist_disp_vect[0]**2 + dist_disp_vect[1]**2
        denominator = math.sqrt(a_mag_squared*b_mag_squared)
        theta = math.acos(dot_product / denominator)
        return theta

    def find_nearest_undrawn_pixel(self) :
        x_current, y_current = self.current_pix
        found_pix_to_go = False
        for i in range(1, self.len_x if self.len_x>self.len_y else self.len_y) : # i is number of pixels between current_pix and the comparing-with pixel
            current_phase_angle = math.inf
            for x in range (x_current-i, x_current+i+1) :
                if x<0   or   x>=self.len_x :
                    continue
                for y in range (y_current-i, y_current+i+1) :
                    if y<0   or   y>=self.len_y :
                        continue
                    if (x==x_current   and   y==y_current) :
                        continue
                    if self.undrawn_pixels[x][y]==1 :
                        found_pix_to_go = True
                        disp_vect = [x - x_current, y - y_current]
                        phase_angle = self.find_phase_angle_with(disp_vect)
                        if phase_angle < current_phase_angle :
                            self.current_pix = [x, y]
                            current_phase_angle = phase_angle
            if found_pix_to_go == True :
                self.displacement_vector = [self.current_pix[0] - x_current, self.current_pix[1] - y_current]
                return
 
    def find_easiest_going_to_neighbor(self) :
        current_x, current_y = self.current_pix
        current_phase_angle = math.inf
        found_neighbor = False
        for neighbor_x in range(current_x-1, current_x+2) :
            if neighbor_x<0 or neighbor_x>=self.len_x :
                continue
            for neighbor_y in range(current_y-1, current_y+2) :
                if neighbor_y<0 or neighbor_y>=self.len_y :
                    continue
                if (neighbor_x==current_x   and   neighbor_y==current_y)   or   self.undrawn_pixels[neighbor_x][neighbor_y]==0 :
                    continue
                found_neighbor = True
                disp_vect = [neighbor_x - current_x, neighbor_y - current_y]
                phase_angle = self.find_phase_angle_with(disp_vect)
                if phase_angle < current_phase_angle :
                    self.current_pix = [neighbor_x, neighbor_y]
                    current_phase_angle = phase_angle

        if found_neighbor == True :
            self.displacement_vector = [self.current_pix[0] - current_x,   self.current_pix[1] - current_y]
        return found_neighbor

    def find_all_undrawn_neighbours(self, current_pix, undrawn_pixels) :
        current_x, current_y = current_pix
        neighbours = []
        for neighbor_x in range(current_x-1, current_x+2) :
            if neighbor_x<0 or neighbor_x>=self.len_x :
                continue
            for neighbor_y in range(current_y-1, current_y+2) :
                if neighbor_y<0 or neighbor_y>=self.len_y :
                    continue
                if (neighbor_x==current_x   and   neighbor_y==current_y)   or   undrawn_pixels[neighbor_x][neighbor_y]==0 :
                    continue
                neighbours.append([neighbor_x, neighbor_y])
        return neighbours

    def find_all_possible_pathes (self, current_pix, undrawn_pixels) :
        # remove current_pix from undrawn_pixels list :
        undrawn_pixels[current_pix[0]][current_pix[1]] = 0

        # get all possible nrighbours to go to :
        neighbours = self.find_all_undrawn_neighbours(current_pix, undrawn_pixels)
        print('current pix is: ', current_pix)
        print('neighbours are:', neighbours)


        if len(neighbours) == 0 :
            return [[current_pix]]

        pathes = []
        for i in range(len(neighbours)) :
            neighbour = neighbours[i]

            undrawn_pixels_neighbours = []
            for x in range(len(undrawn_pixels)) :
                undrawn_pixels_neighbours.append([])
                for y in range(len(undrawn_pixels[0])) :
                    undrawn_pixels_neighbours[x].append(undrawn_pixels[x][y])

            pathes_neighbour = self.find_all_possible_pathes(neighbour, undrawn_pixels_neighbours)
            for path_neighbour in pathes_neighbour :
                # add this path to pathes : [currnt_pix, path_neighbour[0], path_neighbour[1], ...]
                pathes.append([current_pix] + path_neighbour)
                
        return pathes

    def find_longest_path(self) :
        pathes = self.find_all_possible_pathes(self.current_pix, self.undrawn_pixels)
        longest_path_i = 0
        for i in range (1, len(pathes)) :
            if len(pathes[i]) > len(pathes[longest_path_i]) :
                longest_path_i = i
        return pathes[longest_path_i]

    def find_sign_of (x):
        if x >= 0:
            return 1
        else:
            return -1

    def create_mov_seq (self, img_path) :
        # generating moving sequence :
        # note that in move seq list: all 3 coordinates are same as we used to in physics
        mov_seq = [[0, 0, 0]] # first move ever, the machine will be initially zeroed
        
        # case png file :
        if img_path[-3:] == 'png' :
            # analysing img pixels :
            pix_arr = plt.imread(img_path)
            self.len_y = len(pix_arr)
            self.len_x = len(pix_arr[0])
            # finding the pixels to be drawn :
            N_undrawn_pixels = 0
            for x in range(self.len_x) :
                self.undrawn_pixels.append([])
                for y in range(self.len_y) :
                    pix = pix_arr[y][x] #the only array called by index arrangment : [y, x]
                    rms_brightness = pix[0]**2 + pix[1]**2 + pix[2]**2
                    if rms_brightness < 0.6075 :
                        (self.undrawn_pixels[x]).append(1)
                        N_undrawn_pixels += 1
                        pix_arr[y][x] = 0, 0, 0, 1
                    else:
                        (self.undrawn_pixels[x]).append(0)
                        pix_arr[y][x] = 1, 1, 1, 1

            # showing the user what exactly is going to be drawn :
            plt.imshow(pix_arr)
            plt.show()
       
        

            # factiorizing dimentions :
            # 1-length pixel of the img = f-length cm of the system
            self.factor = [self.len_x / self.X_axis_length, self.len_y / self.Y_axis_length]
        
        
            # longest path method :
            '''while (N_undrawn_pixels > 0):
                # find nearest undrawn_pixel to current_pix:
                self.find_nearest_undrawn_pixel()
                # move to it (head-off) :
                current_pix_x, current_pix_y = self.current_pix
                mov_seq.append([current_pix_x, current_pix_y, 1])
            
                path = self.find_longest_path()
                
                for pix in path :
                    N_undrawn_pixels -= 1
                    mov_seq.append([pix[0], pix[1], 0])
                    self.undrawn_pixels[pix[0]][pix[1]] = 0
                self.current_pix = path[-1]'''

            # least displacement vector change method :
            while (N_undrawn_pixels > 0):
                # find nearest undrawn_pixel to current_pix:
                self.find_nearest_undrawn_pixel()
                # move to it (head-off) :
                current_pix_x, current_pix_y = self.current_pix
                mov_seq.append([current_pix_x, current_pix_y, 1])
            
                # make each connected train of pixels in a loop:
                while 1 :
                    current_pix_x, current_pix_y = self.current_pix
                    # remove current_pix from undrawn_pixels array :
                    self.undrawn_pixels[current_pix_x][current_pix_y] = 0
                    N_undrawn_pixels -= 1
                    # you should go to the nigbour undrawn_pixel that costs you minimum changes in velocity magnidude and direction :
                    found_undrawn_neghbour = self.find_easiest_going_to_neighbor()
                    if found_undrawn_neghbour == True :
                        current_pix_x, current_pix_y = self.current_pix
                        mov_seq.append([current_pix_x, current_pix_y, 0])
                    else:
                        break
       
        # case svg file :
        # factor is considered to be 0.5 !
        if img_path[-3:] == 'svg' :
            self.factor = [0.5, 0.5] # pixels per step
            doc = minidom.parse(img_path)  # parseString also exists
            path_strings = [path.getAttribute('d') for path
                            in doc.getElementsByTagName('path')]
            doc.unlink()
            draw = path_strings[0]

            i=0
            current_pos = [0,0]
            last_current_pos = [0,0]
            paths = [[]]
            while i<len(draw) :
                # moveto :
                if draw[i] == 'M' :
                    i += 2
                    first_M = True
                    while 1 :
                        seperator_index = draw[i:].index(',') + i
                        x = float(draw[i : seperator_index])
                        end_of_y = draw[seperator_index:].index(' ') + seperator_index
                        y = float(draw[seperator_index+1 : end_of_y])
                        i = end_of_y + 1
                        # move to it :
                        if first_M :
                            mov_seq.append([x,y,1])
                            first_M = False
                            last_current_pos = [x,y]
                        else :
                            mov_seq.append([x,y,0])
                        paths[-1].append([x,y])
                        current_pos = [x,y]
                        if '-0123456789'.find(draw[i]) == -1 :
                            break

                elif draw[i] == 'm' :
                    i += 2
                    first_M = True
                    while 1 :
                        seperator_index = draw[i:].index(',') + i
                        x = float(draw[i : seperator_index]) + current_pos[0]
                        end_of_y = draw[seperator_index:].index(' ') + seperator_index
                        y = float(draw[seperator_index+1 : end_of_y]) + current_pos[1]
                        i = end_of_y + 1
                        # move to it :
                        if first_M :
                            mov_seq.append([x,y,1])
                            first_M = False
                            last_current_pos = [x,y]
                        else :
                            mov_seq.append([x,y,0])
                        paths[-1].append([x,y])
                        current_pos = [x,y]
                        if '-0123456789'.find(draw[i]) == -1 :
                            break

                # lineto : works fine
                elif draw[i] == 'L' :
                    i += 2
                    while 1 :
                        seperator_index = draw[i:].index(',') + i
                        x = float(draw[i : seperator_index])
                        end_of_y = draw[seperator_index:].index(' ') + seperator_index
                        y = float(draw[seperator_index+1 : end_of_y])
                        i = end_of_y + 1
                        # move to it , head on
                        mov_seq.append([x,y,0])
                        paths[-1].append([x,y])
                        if '-0123456789'.find(draw[i]) == -1 :
                            #last_current_pos = current_pos
                            current_pos = [x,y]
                            break

                elif draw[i] == 'l' :
                    i += 2
                    while 1 :
                        seperator_index = draw[i:].index(',') + i
                        x = float(draw[i : seperator_index]) + current_pos[0]
                        end_of_y = draw[seperator_index:].index(' ') + seperator_index
                        y = float(draw[seperator_index+1 : end_of_y]) + current_pos[1]
                        i = end_of_y + 1
                        # move to it , head on
                        mov_seq.append([x,y,0])
                        paths[-1].append([x,y])
                        current_pos = [x,y]
                        if '-0123456789'.find(draw[i]) == -1 :
                            #last_current_pos = current_pos
                            break

                # curveto :
                elif draw[i] == 'C' :
                    points_since_last_change_of_current_pos = 0
                    X = [current_pos[0]]
                    Y = [current_pos[1]]
                    i += 2
                    while 1 :
                        seperator_index = draw[i:].index(',') + i
                        x = float(draw[i : seperator_index])
                        end_of_y = draw[seperator_index:].index(' ') + seperator_index
                        y = float(draw[seperator_index+1 : end_of_y])
                        i = end_of_y + 1
                        X.append(x)
                        Y.append(y)
                        points_since_last_change_of_current_pos += 1
                        if points_since_last_change_of_current_pos == 3 :
                            current_pos = [x,y]
                            points_since_last_change_of_current_pos = 0
                            P = find_Bezier_points(X, Y, 100)
                            for p in P :
                                mov_seq.append([p[0], p[1], 0])
                                paths[-1].append([p[0], p[1]])
                            X = [current_pos[0]]
                            Y = [current_pos[1]]
                        if '-0123456789'.find(draw[i]) == -1 :
                            if len(X) > 1 :
                                print('EEEEEEEEEEEEEEEEEEEEEEEE')
                                while 1:
                                    pass
                            #last_current_pos = current_pos
                            current_pos = [x,y]
                            break

                elif draw[i] == 'c' :
                    points_since_last_change_of_current_pos = 0
                    X = [current_pos[0]]
                    Y = [current_pos[1]]
                    i += 2
                    while 1 :
                        seperator_index = draw[i:].index(',') + i
                        x = float(draw[i : seperator_index]) + current_pos[0]
                        end_of_y = draw[seperator_index:].index(' ') + seperator_index
                        y = float(draw[seperator_index+1 : end_of_y]) + current_pos[1]
                        i = end_of_y + 1
                        X.append(x)
                        Y.append(y)
                        points_since_last_change_of_current_pos += 1
                        if points_since_last_change_of_current_pos == 3 :
                            current_pos = [x,y]
                            points_since_last_change_of_current_pos = 0
                            P = find_Bezier_points(X, Y, 100)
                            for p in P :
                                mov_seq.append([p[0], p[1], 0])
                                paths[-1].append([p[0], p[1]])
                            X = [current_pos[0]]
                            Y = [current_pos[1]]
                        
                        if '-0123456789'.find(draw[i]) == -1 :
                            #last_current_pos = current_pos
                            if len(X) > 1 :
                                print('EEEEEEEEEEEEEEEEEEEEEEEE')
                                while 1:
                                    pass
                            current_pos = [x,y]
                            break
                
                # closepath : works fine
                elif draw[i] == 'z' or draw[i] == 'Z' :
                    mov_seq.append([last_current_pos[0],last_current_pos[1],0])
                    paths[-1].append([last_current_pos[0],last_current_pos[1]])
                    paths.append([])
                    i += 2
                    current_pos[0] = last_current_pos[0]
                    current_pos[1] = last_current_pos[1]

                # horizontal lineto : works fine
                elif draw[i] == 'H' :
                    i += 2
                    while 1 :
                        end_of_x = draw[i:].index(' ') + i
                        x = float(draw[i : end_of_x])
                        mov_seq.append([x, current_pos[1], 0])
                        paths[-1].append([x, current_pos[1]])
                        i = end_of_x + 1
                        if '-0123456789'.find(draw[i]) == -1 :
                            #last_current_pos = current_pos
                            current_pos = [x,current_pos[1]]
                            break

                elif draw[i] == 'h' :
                    i += 2
                    while 1 :
                        end_of_x = draw[i:].index(' ') + i
                        x = float(draw[i : end_of_x]) + current_pos[0]
                        mov_seq.append([x, current_pos[1], 0])
                        paths[-1].append([x, current_pos[1]])
                        i = end_of_x + 1
                        current_pos = [x,current_pos[1]]
                        if '-0123456789'.find(draw[i]) == -1 :
                            #last_current_pos = current_pos
                            break

                # vertical lineto : works fine
                elif draw[i] == 'V' :
                    i += 2
                    while 1 :
                        end_of_y = draw[i:].index(' ') + i
                        y = float(draw[i : end_of_y])
                        mov_seq.append([current_pos[0], y, 0])
                        paths[-1].append([current_pos[0], y])
                        i = end_of_y + 1
                        if '-0123456789'.find(draw[i]) == -1 :
                            #last_current_pos = current_pos
                            current_pos = [current_pos[0],y]
                            break

                elif draw[i] == 'v' :
                    i += 2
                    while 1 :
                        end_of_y = draw[i:].index(' ') + i
                        y = float(draw[i : end_of_y]) + current_pos[1]
                        mov_seq.append([current_pos[0], y, 0])
                        paths[-1].append([current_pos[0], y])
                        i = end_of_y + 1
                        current_pos = [current_pos[0],y]
                        if '-0123456789'.find(draw[i]) == -1 :
                            #last_current_pos = current_pos
                            break
    
                else :
                    print('wrong char! : ', draw[i], '   ', i)
                    print(draw[0:i])
                    print(draw[i:])
                    while 1:
                        pass
        
            '''
            mov_seq_2 = [[0,0,0]]
            # convert these pathes into adv_seq mode:
            for path in paths :
                path = self.conv_path_to_adv_mode(path)

            for i in range(len(paths)) :
                path_current = paths[i]
                path_next = paths[i+1]
                outer_and_inner_paths = plotter.find_outer_and_inner_path(path_current, path_next)
                if outer_and_inner_paths != False :
                    path_outer, path_inner, refrence_zero = outer_and_inner_paths
                    # draw inner and outer paths:
                    plotter.add_path_to_mov_seq(mov_seq_2, path_inner)
                    plotter.add_path_to_mov_seq(mov_seq_2, path_outer)
                    ### assuming a refrence zero at the middle of the outer path's surrounding rectangle:    ###
                    ### starting from the outer path's home segment, draw sub-paths that are a copy of       ###
                    ### the outer path, each one is less in dimentions that the one before it by the radious ###
                    ### of the head tool.                                                                    ###
                    ### if one of these sub-paths goes through the inner path, lift the head off untill the  ###
                    ### inner path is passed, and put it on after that.                                      ###
                    
                    # we are currently on the outer path's home segment - note last call of "add_path_to_mov_seq" -
                    # start drawing te mentioned sub-paths:
                    sub_path = plotter.refrece_path_to(path_outer, refrence_zero)
                    outer_path_refrenced_at_outers_middle = plotter.refrece_path_to(path_inner, refrence_zero)
                    while 1:
                        pass'''



        return mov_seq


    '''def add_sub_path_to_mov_seq_and_avoid_inner_path(mov_seq, sub_path, inner_path):
        for segment in sub_path:
            x, y = segment
            if 


            mov_seq.append([])'''

    def refrece_path_to(path, point):
        path_outer_middle_refrenced = []
        for segment in path:
            x = segment[0] - point[0]
            y = segment[1] - point[1]
            path_outer_middle_refrenced.append([x,y])
        return path_outer_middle_refrenced

    def add_path_to_mov_seq(mov_seq, path):
        # go to first segment in path with head off:
        mov_seq.append([path[0][0], path[0][1], 1])
        # follow the path, head on
        for segment in path:
            mov_seq.append([segment[0], segment[1], 0])

    def find_outer_and_inner_path(path1, path2):
        # if the two paths are one inside the other, this function will return:
        # [outer, inner, middle point of the outer path's surrounding rectangle]
        # if not, it will return false
        # find diameter corners of each path:
        x_min1, y_min1, x_max1, y_max1 = plotter.find_path_diameter_points(path1)
        x_min2, y_min2, x_max2, y_max2 = plotter.find_path_diameter_points(path2)
        if (x_min1 <= x_min2   and
            y_min1 <= y_min2   and
            x_max1 >= x_max2   and
            y_max1 >= y_max2):
            # path2 is inside path1:
            refrence_zero = [(x_min1+x_max1)/2, (y_min1+y_max1)/2]
            return [path1, path2, refrence_zero]
        
        elif (x_min2 <= x_min1   and
            y_min2 <= y_min1   and
            x_max2 >= x_max1   and
            y_max2 >= y_max1):
            # path1 is inside path2:
            refrence_zero = [(x_min2+x_max2)/2, (y_min2+y_max2)/2]
            return [path2, path1, refrence_zero]

        else :
            return False


           

    def find_path_diameter_points(path):
        x_min, y_min, x_max, y_max = math.inf, math.inf, 0, 0
        for segment in path:
            x, y = segment
            if x > x_max:
                x_max = x
            if x < x_min:
                x_min = x
            if y > y_max:
                y_max = y
            if y < y_min:
                y_min = y
        return [x_min, y_min, x_max, y_max]

    def conv_path_to_adv_mode(self, path):
        adv_mov_seq = []
        f_x, f_y = self.factor
        for i in range(1, len(path)) :
            current_pos = path[i-1]
            next_pos = path[i]
            current_pos_x, current_pos_y = current_pos
            next_pos_x, next_pos_y = next_pos

            delta_y = next_pos_y - current_pos_y    # in pixels
            delta_x = next_pos_x - current_pos_x    # in pixels
            dy = float()                            # in steps
            dx = float()                            # in steps
            if delta_y == 0 :
                dy = 0
                dx = self.delta * plotter.find_sign_of(delta_x)
            elif delta_x == 0 :
                dx = 0
                dy = self.delta * plotter.find_sign_of(delta_y)
            else :
                if abs(delta_y)/f_y > abs(delta_x)/f_x :
                    dy = self.delta * plotter.find_sign_of(delta_y)
                    dx = self.delta * abs(delta_x / delta_y) * plotter.find_sign_of(delta_x)
                else :
                    dx = self.delta * plotter.find_sign_of(delta_x)
                    dy = self.delta * abs(delta_y / delta_x) * plotter.find_sign_of(delta_y)

            y_remaining = abs(delta_y/f_y)
            x_remaining = abs(delta_x/f_x)
            y_current = current_pos_y/f_y
            x_current = current_pos_x/f_x
        
            while y_remaining > 0   or   x_remaining > 0 :
                adv_mov_seq.append([x_current, y_current])
                if x_remaining > 0 :
                    x_current += dx
                    x_remaining -= abs(dx)
                if y_remaining > 0 :
                    y_current += dy
                    y_remaining -= abs(dy)
        return adv_mov_seq

    def create_adv_mov_seq(self, move_seq) :
        adv_mov_seq = []
        f_x, f_y = self.factor
        last_head_state = 1 # because first head state in list is 0
        for i in range(1, len(move_seq)) :
            current_pos = move_seq[i-1]
            next_pos = move_seq[i]
            current_pos_x, current_pos_y, _ = current_pos
            next_pos_x, next_pos_y, head_state = next_pos

            if head_state != last_head_state :
                last_head_state = head_state
                if head_state == 0 :
                    adv_mov_seq.append('h')
                else :
                    adv_mov_seq.append('H')
        
            delta_y = next_pos_y - current_pos_y    # in pixels
            delta_x = next_pos_x - current_pos_x    # in pixels
            dy = float()                            # in steps
            dx = float()                            # in steps
            if delta_y == 0 :
                dy = 0
                dx = self.delta * plotter.find_sign_of(delta_x)
            elif delta_x == 0 :
                dx = 0
                dy = self.delta * plotter.find_sign_of(delta_y)
            else :
                if abs(delta_y)/f_y > abs(delta_x)/f_x :
                    dy = self.delta * plotter.find_sign_of(delta_y)
                    dx = self.delta * abs(delta_x / delta_y) * plotter.find_sign_of(delta_x)
                else :
                    dx = self.delta * plotter.find_sign_of(delta_x)
                    dy = self.delta * abs(delta_y / delta_x) * plotter.find_sign_of(delta_y)

            y_remaining = abs(delta_y/f_y)
            x_remaining = abs(delta_x/f_x)
            y_current = current_pos_y/f_y
            x_current = current_pos_x/f_x
        
            while y_remaining > 0   or   x_remaining > 0 :
                adv_mov_seq.append([x_current, y_current])
                if x_remaining > 0 :
                    x_current += dx
                    x_remaining -= abs(dx)
                if y_remaining > 0 :
                    y_current += dy
                    y_remaining -= abs(dy)
        return adv_mov_seq

    def create_instructions(self, adv_mov_seq) :
        instructions = ['3', 'H'] # start by going home and lifting the head off
        current_x = float()
        current_y = float()
        x_at_last_step = 0
        y_at_last_step = 0

        for mov in adv_mov_seq :
            if mov == 'h'   or   mov=='H' :
                instructions.append(mov)
                continue
        
            current_x, current_y = mov

            if current_x - x_at_last_step >= 1 :
                instructions.append('X')
                x_at_last_step += 1

            elif current_x - x_at_last_step <= -1 :
                instructions.append('x')
                x_at_last_step -= 1

            if current_y - y_at_last_step >= 1 :
                instructions.append('Y')
                y_at_last_step += 1

            elif current_y - y_at_last_step <= -1 :
                instructions.append('y')
                y_at_last_step -= 1

        #instructions.append('2') #soft zero at the end of the plot
        count = 0
        for i in instructions :
            if i=='h' or i=='H' :
                count+=1
        print(count)
        instructions.append('H')
        return instructions

    def send_instructions(self, instructions) :
        self.connect_controller()
        last_reached_percentage = 0
        N_total_instructions = len(instructions)
        N_sent_instructions = 0
        x_current = 0
        y_current = 0
        for instruction in instructions :
            self.controller.write(bytes(instruction, 'utf-8'))
            # wait for controller's response:
            self.controller.inWaiting()
            response = chr((self.controller.read())[0])
            if instruction == 'H' or instruction=='h' :
                time.sleep(self.head_interval)
            N_sent_instructions += 1
            if instruction == 'X' :
                x_current += 1
            elif instruction == 'x' :
                x_current -= 1
            if instruction == 'Y' :
                y_current += 1
            elif instruction == 'y' :
                y_current -= 1
            print('sending instructions to controller[' + '█'*last_reached_percentage + '.'*(50-last_reached_percentage) + '] [x,y]= ', [x_current,y_current], sep='', end='            \r')
            if (N_sent_instructions*50)//N_total_instructions > last_reached_percentage :
                last_reached_percentage += 1
            if response != instruction :
                print(100*' ')
                print('Bad controller response!')
                print('[x,y]= ', [x_current,y_current])
                while 1:
                    pass
        print('sending instructions to controller[' + '█'*50 + ']', sep='')
        print('done processing image ✓')

    def simulate_instructions(self, instructions) :
        fig, ax = plt.subplots()
        previous_X_drawed_lists = []
        previous_Y_drawed_lists = []
        current_x = 0
        current_y = 0
        head_state = False
        _i = 0
        while _i < len(instructions) :
            head_state = True if instructions[_i]=='h' else False
            X_drawed_list = []
            Y_drawed_list = []
            while 1 :
                _i = _i + 1
                if _i ==  len(instructions) :
                    break
                instruction = instructions[_i]
                
                if instruction=='h'   or   instruction=='H' :
                    print('          ', instruction)
                    time.sleep(0.25)
                    break
                print( instruction )
                if instruction == 'X' :
                    current_x+=1
                elif instruction == 'x' :
                    current_x-=1
                elif instruction == 'Y' :
                    current_y+=1
                elif instruction == 'y' :
                    current_y-=1
                plt.gca().cla()
                ax.set_xlim(0, self.X_axis_length)
                ax.set_ylim(0, self.Y_axis_length)
                
                if head_state == True :
                    ax.plot(current_x, current_y, marker="s")
                    X_drawed_list.append(current_x)
                    Y_drawed_list.append(current_y)
                else :
                    ax.plot(current_x, current_y, marker="o")
                ax.plot(X_drawed_list, Y_drawed_list, marker=",")
                for i in range (len(previous_X_drawed_lists)) :
                    ax.plot(previous_X_drawed_lists[i], previous_Y_drawed_lists[i], marker=",")
                plt.gca().invert_yaxis()
                fig.canvas.draw()
                plt.pause(0.01)
            previous_X_drawed_lists.append(X_drawed_list)
            previous_Y_drawed_lists.append(Y_drawed_list)
        plt.show()
    
    def simulate_plot(self, img_path) :
        mov_seq = self.create_mov_seq(img_path)
        adv_mov_seq = self.create_adv_mov_seq(mov_seq)

        instructions = self.create_instructions(adv_mov_seq)
              
        self.simulate_instructions(instructions)
        
    def plot_img(self, img_path) :
        # create simple mov_seq :
        mov_seq = self.create_mov_seq(img_path)

        # edit mov_seq so it gets compatible with stepper motors :
        adv_mov_seq = self.create_adv_mov_seq(mov_seq)

        # create instructions for the generated move_seq :
        instructions = self.create_instructions(adv_mov_seq)

        # send those instructions :
        self.send_instructions(instructions)

p = plotter('COM5', 9600, 480, 480, 0.1)
#p.manual_movement()
#p.plot_img('img.svg')
p.simulate_plot('img.svg')

