molstr = [' C', char(10), ...
' C 1 B1', char(10), ...
' H 1 B2    2 A1', char(10), ...
' H 1 B3    2 A2    3 D1', char(10), ...
' H 1 B4    2 A3    3 D2', char(10), ...
' H 2 B5    1 A4    3 D3', char(10), ...
' H 2 B6    1 A5    3 D4', char(10), ...
' H 2 B7    1 A6    3 D5', char(10), ...
' ', char(10), ...
'   B1      =       1.54', char(10), ...
'   B2      =       1.07', char(10), ...
'   B3      =       1.07', char(10), ...
'   B4      =       1.07', char(10), ...
'   B5      =       1.07', char(10), ...
'   B6      =       1.07', char(10), ...
'   B7      =       1.07', char(10), ...
'   A1      =       109.5', char(10), ...
'   A2      =       109.5', char(10), ...
'   A3      =       109.5', char(10), ...
'   A4      =       109.5', char(10), ...
'   A5      =       109.5', char(10), ...
'   A6      =       109.5', char(10), ...
'   D1      =       120.0', char(10), ...
'   D2      =      -120.0', char(10), ...
'   D3      =       180.0', char(10), ...
'   D4      =        60.0', char(10), ...
'   D5      =       -60.0', char(10)];

basisname = '6-31+g*';
%basisname = 'sto-3g';

matpsi = MatPsi({molstr, basisname});
