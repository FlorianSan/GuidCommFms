clc 
clear

M = readmatrix('file.txt');
t = M(:,1); %s
xtk = M(:,2); %Nm
roll = M(:,3);%°
heading = M(:,4);%°

figure(1)
pap = tiledlayout(3,1);

nexttile
plot(t, roll*180/pi)
title("Roulis avion")
ylabel('Angle de roulis (°)')
xlabel('Temps (s)')

nexttile
plot(t, xtk)
title("Distance erreur")
ylabel('XTK (m)')
xlabel('Temps (s)')

nexttile
plot(t, heading*180/pi)
title("Cap avion")
ylabel('Cap (°)')
xlabel('Temps (s)')

% Enregistrement
saveas(pap,'Images/identification_avion.png','png');

Ts = 0.1;
y = [xtk, heading];

data = iddata(y,roll,Ts);
nx = 1:10;
sys = ssest(data,nx);


P = pole(sys);
p = [-0.5, -10];
K = place(sys.A,sys.B,p);
disp(K)

modele = sim('bankAngleXtkTae');
figure(2)
aff_sim = tiledlayout(2,1);

nexttile
plot(modele.simout.time, modele.simout.signals.values(:,1))
title("Distance erreur bouclé")
ylabel('XTK (m)')
xlabel('Temps (s)')

nexttile
plot(modele.simout.time, modele.simout.signals.values(:,2))
title("Cap avion bouclé")
ylabel('Cap (°)')
xlabel('Temps (s)')

% Enregistrement
saveas(aff_sim,'Images/Sortiebouclage.png','png');

