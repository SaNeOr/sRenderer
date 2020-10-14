## Scan Convert Line

笔记不见了= =..算了...




这一章有一个magic(math)用来计算三角形的质心坐标 虽然不太理解.. 但很好用...

    u.AB + v.AC + .PA = .0;
=>  u.AB_x + v.AC_x + .PA.x = .0
=>  u.AB_y + v.AC_y + .PA.y = .0

(.AB_x, .AC_X, .PA_X) X (.AB_y, AC_y, PA_y) = u;

P_barycentric = (1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);


