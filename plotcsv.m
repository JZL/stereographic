figure(1)
points = csvread('out.pgm');
points(:,1);
scatter3(points(:,1),points(:,2),points(:,3))
% points = dlmread('points');
% scatter(points(:,1),points(:,2))