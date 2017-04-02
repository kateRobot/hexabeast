

% TASK #1
% AAS - 2017.S1.
% Kate Apel.


function trialLaserProgram(file)

    % load data file.
    if ~exist('file','var'), file ='Laser__2.mat'; end;
    load(file); 
    
    % -----------------------Plot using handles------------------------
    
    % Create graphical object for refreshing data during program execution.
    figure(1) ; clf();                          % create blank graphical object
    myGUIHandles.handle1 = plot(0,0,'b.');      % laser points    
    myGUIHandles.handle2 = title('');  
    myGUIHandles.handle4 = plot(0,0,'r.');      % ObjectsOI
    myGUIHandles.handle5 = plot(0,0,'g*');      % Brilliant centres
    axis([-10,10,0,10]);                        % RangeOI
    xlabel('x  (metres)');
    ylabel('y  (metres)');  
    zoom on ;  grid on;
    
    %---------------Define scans and loop through scan data------------
    
    % For a data structure saved as "dataL"
    N = dataL.N;                      % number of scans in this squence.
    disp('Doing the thing...');
    skip=3;
    for i=1:skip:1
        
        % Native time expressed via uint32 numbers, where 1 unint 
        % means 1/10000 second (i.e. 0.1 millisecond)
        % t is time elapsed in seconds from first scan
        t =  double(dataL.times(i)-dataL.times(1))/10000; 
        scan_i = dataL.Scans(:,i);
        ProcessScan(scan_i,t,myGUIHandles,i);
        pause(0.01) ;                   % wait for ~10ms
    end;
    disp('Finished doing the thing. Bye!');

return;

end


function ProcessScan(scan_i,t,myGUIHandles,i)

    % ---------------Extract range, intensity of pixels------------

    % Extract intensities, by taking last 4 bits of pixel
    maskE000 = bitshift(uint16(7),13);
    intensities = bitand(scan_i,maskE000);
    ii = find(intensities~=0);
    
    % Extract ranges taking first 12 bits of pixel (expressed in meters)
    mask1FFF = uint16(2^13-1);
    ranges   = single(bitand(scan_i,mask1FFF))*0.01; 

    % Turn range info into 2D points, expressed in Cartesian
    angles = [0:360]'*0.5* pi/180 ;
    X = cos(angles).*ranges;
    Y = sin(angles).*ranges;
    X
    Y
    % Handle 1: laser points in blue
    set(myGUIHandles.handle1,'xdata',X,'ydata',Y);
    %---- Identify clusters, classify OOIs, Plot and Print --------

    OOIs = ExtractOOIs(X,Y,intensities) ;
    OOIs
    PlotOOIs(OOIs,myGUIHandles,t,i,X,Y,ii);

return;

end

function r = ExtractOOIs(X,Y,intensities)
    
    % Set up arrays for OOI information
    r.N = 0;
    r.Centers = [];
    r.CentersX = [];
    r.CentersY = [];
    r.Sizes   = [];
    r.Colours = [];

    % Define two empty arrays for start and end points of clusters
    starts  = zeros(1,361);
    ends = zeros(1,361);
    currentcluster = 1; 
    starts(currentcluster) = 1;
    
    % Identify clusters by proximity between points i and i-1
    for i = 2:361
        distToNext = sqrt((X(i)-X(i-1)^2)+(Y(i)-Y(i-1)^2));
        if distToNext>=0.1
            ends(currentcluster)= i-1;
            currentcluster = currentcluster + 1;
            starts(currentcluster) = i;
        end
       
    end

    ends(currentcluster) = 361;
    numclusters = currentcluster;
    numclusters
    
    % Classify if cluster is OOI based on length (diameter) of cluster
    %currentcluster = 1;
    for i = 1:numclusters
        j=starts(i);
        k=ends(i);
        clusterDiameter = sqrt((X(k)-X(j))^2+(Y(k)-Y(j))^2);
        % If diameter matches criteria, cluster is OOI
        if clusterDiameter>5 && clusterDiameter<20
            % If cluster is OOI, gather information on OOI
            r.N = r.N+1;
            r.Centers(i,:) = [((X(k)+X(j))/2) ((Y(k)+Y(j)/2)),];
            r.CentersX(i,:) = [X(k)+X(j)/2];
            r.CentersY(i,:) = [Y(k)+Y(j)/2];
            r.Sizes(i,:)   = clusterDiameter;
            if any(intensities(j:k))
                r.Colours(r.N,:) = 1;
            else 
                r.Colours(r.N,:) = 0;
            end
            r.N
            clusterDiameter
            
        end
        
        
    end
    currentcluster = currentcluster + 1;
    
    
return;

end
    
function PlotOOIs(OOIs,myGUIHandles,t,i,X,Y,ii)

    if OOIs.N<1 
        return ; 
    else
        
        % Handle 1: laser points in blue
        set(myGUIHandles.handle1,'xdata',X,'ydata',Y);
        
        % Handle 2: title
        s= sprintf('Laser scan # [%d] at time [%.3f] secs',i,t);
        set(myGUIHandles.handle2,'string',s);
        
        % Handle 4: OOIs in red
        set(myGUIHandles.handle4,'xdata',X(ii),'ydata',Y(ii));
        
        % Handle 5: Brilliant centres in green stars
        xdata = OOIs.CentersX;
        ydata = OOIs.CentersY;
        set(myGUIHandles.handle5,'xdata',xdata,'ydata',ydata);

    end
return;

end



% Questions? Ask the boss j.guivant@unsw.edu.au

