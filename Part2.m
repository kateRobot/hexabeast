function Part2()

clc(); close all;

t = tcpip('localhost', 12345, 'NetworkRole','client');
t.InputBufferSize = 500000;
t.ByteOrder = 'littleEndian';

% --------------------------------------
% Create graphical object for refreshing data during program execution.
figure(1) ; clf(); hold on;

MyGUIHandles.handle1 = plot3(0,0,0,'.','MarkerSize',1);      % to be used for showing the camera points
axis([0 1500 -750 750 -750 750]);                         % focuses the plot on this region (of interest, close to the robot)
xlabel('z(mm)');
ylabel('x(mm)');
zlabel('y(mm)');

MyGUIHandles.handle4 = title('');           % create an empty title..
zoom on ;  grid on;
view([-1 0 0]);

frame = 0;

fopen(t);

while(1)
    %!!! read in array and frame number from shared memory via tcp
    % establish connection through tcp
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
        set(MyGUIHandles.handle1,'xdata',Z,'ydata',-X,'zdata',Y);
        % and some text...
        s = sprintf('3D Camera Frame # [%d]',frame);
        set(MyGUIHandles.handle4,'string',s);
    pause(0.1) ;                   % wait for ~1s
    frame = frame + 1;
        
end;
fclose(t);
disp('Done. Bye.');

return;
end