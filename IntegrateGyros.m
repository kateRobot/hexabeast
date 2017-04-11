% Andrew Wang (z3466315)
%
function IntegrateGyros
clc(); close all; clear all;
delete(instrfindall);

SPACE = 32;
NEWLINE = 10;

%replace with Arduino input
%     MyFile = 'G:\Part5\IMU_data.mat';  
%     MyFile = '/Volumes/NO NAME/Part5/IMU_data.mat';
% 
%     load(MyFile) ;
% 
%     times = double(IMU.times);
%     times = times/10000;
%     times = times - times(1) ; 


%arduino=serial('COM3','BaudRate',9600);
arduino=serial('COM7','BaudRate',9600);
fopen(arduino);

times=[1:32];
raw_signal=zeros(3, length(times));
for i=1:length(times)
%      [raw_signal(1), raw_signal(2), raw_signal(3)]=fscanf(arduino,'%d %d %d');
    raw_signal(:,i)=fscanf(arduino,'%d %d %d\n');    
    pause(0.2);
end

	
% fclose(arduino);
% disp('making plot..')
% plot(times,signal);
%----------------------------------

% IMU is a structure, whose field "DATAf" contains the Gyros' measurements
% IMU.DATAf(1,:) is Alphax  (local Roll acc)
% IMU.DATAf(2,:) is Alphay  (local Pitch acc)
% IMU.DATAf(3,:) is Alphaz  (local Yaw acc)
% IMU.DATAf(4,:) is Wx  (local Roll rate)
% IMU.DATAf(5,:) is Wx  (local Pitch rate)
% IMU.DATAf(6,:) is Wx  (local Yaw rate)
% All measurements are expressed in radians/second

%replace with Arduino input
%     wx = IMU.DATAf(4,:);
%     wy = IMU.DATAf(5,:);
%     wz = IMU.DATAf(6,:);

%'signal' is of string type
% ascii_signal = zeros(1, 8);
% edge1 = 1;
% edge2 = edge1;      %edge1 and edge2 follow temp_char
% temp_char = zeros(1, 2);
% nth_signal = 1;     %nth signal: corresponds to roll, pitch, yaw respectively
% for i = 1:length(raw_signal)
%     ascii_signal = double(raw_signal(i));
%     for j = 1:length(ascii_signal)
%         if ascii_signal(j) ~= SPACE || ascii_signal(j) ~= NEWLINE
%             temp_char(edge1) = ascii_signal(j);
%             edge1 = edge1 + 1;
%         elseif ascii_signal(j) == SPACE
%             processed_signal(nth_signal) = num2double(char(temp_char));
%             edge1 = 1;
%             edge2 = edge1;
%             nth_signal = nth_signal + 1;
%         elseif ascii_signal(j) == NEWLINE
%             
%         end
%     end
%     
% end


% wx = raw_signal(1);
% wy = raw_signal(2);
% wz = raw_signal(3);

%----------------------------------

%To find bias
% Biasx = mean(wx(1:3000));
% Biasy = mean(wy(1:3000));
% Biasz = mean(wz(1:3000));
%disp(Biasz);]

Biasx = 0;
Biasy = 0;
Biasz = 0;

%replace with Arduino input
%     Wx = IMU.DATAf(4,:) - Biasx;
%     Wy = IMU.DATAf(5,:) - Biasy;
%     Wz = IMU.DATAf(6,:) - Biasz;
    
%For 2D integration only:
Wx = raw_signal(1,:) - Biasx;
Wy = raw_signal(2,:) - Biasy;
Wz = raw_signal(3,:) - Biasz;
%----------------------------------

%Initialise Attitude
roll = zeros(1, length(times)); 
pitch = zeros(1,length(times)); 
yaw = zeros(1,length(times));
%Attitude = [];



i = 2;
while times(i) < times(end)
    dt = times(i) - times(i-1);
        
    roll(i) = roll(i-1) + dt * (Wx(i-1) + (Wy(i-1)*sin(roll(i-1)) + Wz(i-1)*cos(roll(i-1)))*tan(pitch(i-1))) ; 
    pitch(i) = pitch(i-1) + dt * (Wy(i-1)*cos(roll(i-1)) - Wz(i-1)*sin(roll(i-1)));
    yaw(i) = yaw(i-1) + dt * (Wy(i-1)*sin(roll(i-1)) + Wz(i-1)*cos(roll(i-1))/cos(pitch(i-1)));
    
    %Attitude = [Roll_v, Pitch_v, Yaw_v];
    
    i = i+1;
end

%---------------------------------------------------
% -- Here we plot the 3 angular rates (local, from gyros) ------

%let the user choose/display both modes
choice = input('Please select Mode\n');
if choice == 1
    disp('Integrating in 2D...');
    plot_yaw_only(times, yaw);
elseif choice == 2
    disp('Integrating in 3D...');
    plot_all(times, roll, pitch, yaw);
else
    disp('Error');
end

end

function plot_all(some_times, some_roll, some_pitch, some_yaw)
figure(1) ;  clf() ; hold on;
k = 180/pi;

subplot(311);
plot(some_times,some_roll*k,'r');
title('Integrated Roll - unbiased');
xlabel('time (in seconds)');
ylabel('angle (degrees)');
grid on ;
zoom on;

subplot(312);
plot(some_times,some_pitch*k,'g');
title('Integrated Pitch - unbiased');
xlabel('time (in seconds)');
ylabel('angle (degrees)');
grid on ;
zoom on;

subplot(313);
plot(some_times,some_yaw*k,'g');
title('Integrated Yaw - unbiased');
xlabel('time (in seconds)');
ylabel('angle (degrees)');
grid on ;
zoom on;

%subplot(times,YawNoBias*k,'b');
%plot(times,Intz*k,'r');
%legend({'Roll', 'Pitch', 'Yaw'});

disp('Done, see the plots in figure 1');

end

function plot_yaw_only(some_times, some_yaw)
figure(1) ;  clf() ; hold on;    
k = 180/pi;
    
plot(some_times,some_yaw*k,'g');
title('Integrated Yaw - unbiased');
xlabel('time (in seconds)');
ylabel('angle (degrees)');
grid on ;
zoom on;  

disp('Done, see the plots in figure 1');

end
