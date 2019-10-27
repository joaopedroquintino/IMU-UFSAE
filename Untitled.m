fileID = fopen('NEWTEST.TXT','r');

formatSpec = '%i';
%mpu = fread(fileID,10,'bit16=>int16'); %Descarta

mpu = [];
trash = [];
ax_deg = [];
ay_deg = [];
az_deg = [];
gx_deg = [];
gy_deg = [];
gz_deg = [];

while(~feof(fileID))
    mpu = fread(fileID,8,'bit16=>int16');
    ax_deg = [ax_deg double(mpu(1:8:end))'];
    ay_deg = [ay_deg double(mpu(2:8:end))']; 
    az_deg = [az_deg double(mpu(3:8:end))'];
    gx_deg = [gx_deg double(mpu(4:8:end))']; 
    gy_deg = [gy_deg double(mpu(5:8:end))'];
    gz_deg = [gz_deg double(mpu(6:8:end))'];
end
    
ax_deg = ax_deg/1000; 
ay_deg = ay_deg/1000;
az_deg = az_deg/1000;
gx_deg = gx_deg/100;
gy_deg = gy_deg/100;
gz_deg = gz_deg/100;

tempo = (1:length(ax_deg))*(1/100);
plot(tempo,ax_deg,'b');

hold on

tempo = (1:length(ay_deg))*(1/100);
plot(tempo,ay_deg,'m');

hold on
tempo = (1:length(az_deg))*(1/100);
plot(tempo,az_deg,'g');

figure

tempo = (1:length(gx_deg))*(1/100);
plot(tempo,gx_deg,'r');

hold on
tempo = (1:length(gy_deg))*(1/100);
plot(tempo,gy_deg,'b');

hold on
tempo = (1:length(gz_deg))*(1/100);
plot(tempo,gz_deg,'k');

fclose(fileID);