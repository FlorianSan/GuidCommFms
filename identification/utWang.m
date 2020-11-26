function [Kp, Ki, Kd, m, w0, dp] = utWang(F, ts, D)
    %Pole dominant
    m = -log(D)/sqrt(pi^2 + log(D)^2);
    w0 = 4/(m*ts);
    %%{
    figure
    step(tf([w0^2], [1 2*m*w0 w0^2]))

    figure
    plot([-m*w0 -m*w0],[-w0*sqrt(1-m^2) w0*sqrt(1-m^2)], 'xr') %pole dominant souhaite
    hold on
    rlocus(-F)
    %%}
    dp = -m*w0+j*w0*sqrt(1-m^2); %pole dominant souhaite

    a = real(dp);
    b = imag(dp);
    Fdp = polyval(F.num{1},dp)/polyval(F.den{1},dp);

    X1 = (1/(2*b))*imag(-1/Fdp)-(1/(2*a))*real(-1/Fdp);
    X2 = (1/(2*b))*imag(-1/Fdp)+(1/(2*a))*real(-1/Fdp);

    L = tf(conv(-F.num{1},[1 -2*a (a^2+b^2)]),conv(2*a*F.den{1},[0, 1, 0]) + conv(F.num{1},[2*a*X2, 0, -2*a*(a^2+b^2)*X1]));
    figure
    subplot(121);
    rlocus(-L)
    title('-L')
    subplot(122);
    rlocus(L)
    title('L')
    
    Kp = input('Kp ? proposition -5.44\n') ;%-5.44;
    Ki = -(a^2+b^2)*((Kp/(2*a))+X1);
    Kd = -(Kp/(2*a)) + X2;
end