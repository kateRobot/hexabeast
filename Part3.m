function Part3()

    %find angle between each pixel by taking image of known width and
    %compare to number of pixels
    
    %also need to find centre of pole 

    clc(); close all;
    
    t = tcpip('localhost', 12345, 'NetworkRole','client');
    t.InputBufferSize = 500000;
    t.ByteOrder = 'littleEndian';

    fopen(t);

    frame = 0;

    %fopen(t,'r');
    while(1)
        %!!! read in array and frame number from shared memory via tcp
        % establish connection through tcp
        
        if frame == 0,
            figure(1) ; clf() ;  
            subplot(2,1,1) ; hold on;
            MyGUIHandles.handle1 = plot3(0,0,0,'b.','MarkerSize',1);      % for edges of OOIs
            MyGUIHandles.handle2 = plot3(0,0,0,'r.','MarkerSize',1);      % for edges of OOIs
            MyGUIHandles.handle3 = plot3(0,0,0,'g+');      % for edges of OOIs
            axis([0 1500 -750 750 -750 750]);                         % focuses the plot on this region (of interest, close to the robot)
            xlabel('z(mm)');
            ylabel('x(mm)');
            zlabel('y(mm)');
            zoom on ;  grid on;
            view([-1 0 0]);
            hold off;
            
            subplot(2,1,2) ; hold on;
            MyGUIHandles.handle4 = plot3(0,0,0,'b.','MarkerSize',5);      % for edges of OOIs
            MyGUIHandles.handle5 = plot3(0,0,0,'g+');      % for edges of OOIs
            MyGUIHandles.handle6 = plot3(0,0,0,'r.','MarkerSize',5);      % for edges of OOIs
            axis([-750 750 0 1000 -750 750]);                         % focuses the plot on this region (of interest, close to the robot)
            xlabel('x(mm)');
            ylabel('z(mm)');
            zlabel('y(mm)');
            zoom on ;  grid on;
            view([0 0 1]);
            hold off;
            
            MyGUIHandles.handle7 = suptitle('');
            
        else   % for, later, using the graphical objects...
            %for j=1:120
                %row = 121-j;
                row = 0;
                X = fread(t,76800, 'short');
                Y = fread(t,76800, 'short');
                Z = fread(t,76800, 'short');
                if X(:) == 32001
                    X(:) = NaN;
                end
                if Y(:) == 32001
                    Y(:) = NaN;
                end
                if Z(:) == 32001
                    Z(:) = NaN;
                end
                
                i = find(Y(:)==row);
        
                
                OOIs = ExtractOOIs(X(i), Z(i));
                
                ii = find(Z(:) > 1000);
                 
                % and then refresh the data in the plots...
                iii = find(Z(:) <= 1000);
                
                Y_temp = zeros(1,OOIs.N);
                for j=0:OOIs.N-1
                    Y_temp = row;
                end

                
                
                set(MyGUIHandles.handle1, 'xdata', Z(ii), 'ydata', X(ii), 'zdata', Y(ii)); %over a metre
                set(MyGUIHandles.handle2, 'xdata', Z(iii), 'ydata', X(iii), 'zdata', Y(iii)); %under a metre range but not OOI
                %set(MyGUIHandles.handle3, 'xdata', OOIs.Centres(:,2), 'ydata', -OOIs.Centres(:,1), 'zdata', Y_temp(:)); %centres
                
                set(MyGUIHandles.handle4, 'xdata', -X(i), 'ydata', Z(i), 'zdata', Y(i)); %under a metre range but not OOI
                %set(MyGUIHandles.handle5, 'xdata', -OOIs.Centres(:,1), 'ydata', OOIs.Centres(:,2), Y_temp(:)); %centres
                %set(MyGUIHandles.handle6, 'xdata', OOIs.left:OOIs.right, 'ydata', depth(row,OOIs.left:OOIs.right)); %OOIs
                % and some text...
                s = sprintf('3D Camera on Row %d, Frame #%d',row, frame);
                set(MyGUIHandles.handle7,'string',s);
                pause(0.1);
            %end
        end

        frame = frame + 1;
        %pause(1) ;                   % wait for ~1s
    end;

    fclose(t);
    disp('Done. Bye.');

    return;
end

function OOI = ExtractOOIs(X,Z)
    OOI.N = 0;
    OOI.Centres = [];
    OOI.left = [];
    OOI.right = [];
    % your part....
    
    edge1 = 1;
    n = 0;
    
    X = X(:);
    Z = Z(:);
    
    %Separate out OOI's
    %Check distance between pixels by dist. formula
    for i = 2:length(X)
%         if (~isnan(Z(i)))
        
        %dist between pixel i and i+1 is stored in dist(i+1)
        dist(i) = sqrt((X(i)-X(i-1))^2 + (Z(i)-Z(i-1))^2);
        
        %To find number of OOIs
        if dist(i) > 100
            %mark cluster boundary
            edge2 = i-1;
            cluster_size = edge2 + 1 - edge1;
            
            %%%%Calculate diameter step----------------
            A = -2*[X(edge1:edge2-1) - X(edge1+1:edge2), Z(edge1:edge2-1) - Z(edge1+1:edge2)];
            C = Z(edge1+1:edge2).^2 - Z(edge1:edge2-1).^2 + X(edge1+1:edge2).^2 - X(edge1:edge2-1).^2;
            centre = A\C;
            radius = mean(sqrt((X(edge1:edge2) - centre(1)).^2 + (Z(edge1:edge2) - centre(2)).^2));
            diameter = 2*radius;
            %------------------------------------------
            
            if diameter > 30 && diameter < 500
                %treat all groups of pixels > 3 as clusters
                if cluster_size > 2
                    n = n + 1;

                    %Store centres
%                     centre = abs(centre);
                     if centre(1) > 1000
                        centre(1) = NaN; 
                     end
                     if centre(2) > 1000
                        centre(2) = NaN; 
                     end
                    
                    OOI.Centres(:,n) = centre;
                end
                %%%%%%
                
            OOI.left = edge1;
            OOI.right = edge2;
                
            end
            edge1 = edge2 + 1;
            OOI.N = n;
%         end
        end
    end
    
return;
end
