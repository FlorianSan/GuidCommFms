clear
close all

%constantes
g = 9.80665; %en m/s^2
KTS2MS = 1852. / 3600.; %knots -> m/s
DEG2RAD = pi/180.; %degres -> rad
NM2M = 1852.; %NM -> m

%vent
W = 1*50*KTS2MS;
psiW = 90*DEG2RAD;

%trim
%Valeur initiale
Ve = 180*KTS2MS; %vitesse de trim en m/s
gammae = 0*DEG2RAD; %en radians
nze = cos(gammae);
Vp = Ve*cos(gammae);


epsTimeScale = 1/5;


tauPhi = 3; %sec


tauCap = tauPhi/epsTimeScale; %sec
k1 = Vp/(g*nze*tauCap);

tauEy = 4*tauCap; %tauCap/epsTimeScale %sec
k2 = Vp/(g*nze*tauCap*tauEy);

k3 = 1/tauPhi;
phiMax = 30*DEG2RAD;




if false
    %Apres simulation
    %Pour afficher la trajectoire dans le plan horizontal
    figure(1)
    plot(yAxe/NM2M, xAxe/NM2M, 'rx')
    hold on
    plot(out.get('yNM').signals.values, out.get('xNM').signals.values);
    grid on
    axis equal
end



