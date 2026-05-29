program test;
var
    a, b, c : integer;
begin
    read(a, b);
    write(a + b);
    c := a + b * 2;
    write(c);
    while a < 10 do
    begin
        a := a + 1;
        write(a)
    end
end.