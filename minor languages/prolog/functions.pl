radian_converter( degmin( Deg, Min), Rad) :-
    Rad is (Deg + Min/60) * pi/180.

haversine(Lat1, Lon1, Lat2, Lon2, D) :-
    Dlon is Lon2 - Lon1,
    Dlat is Lat2 - Lat1,
    A is (sin(Dlat/2))**2 + cos(Lat1) * cos(Lat2) * (sin(Dlon/2))**2,
    C is 2 * atan2(sqrt(A), sqrt(1 - A)),
    D is 3961 * C.

convert_time(Time, Hrs, Min) :-
    Hrs is floor(Time),
    A is Time - Hrs,
    Min is round(A * 60).

format_time(Time) :-
    Time < 10 -> write('0'), write(Time);
    write(Time).

write_time(Time) :-
    convert_time(Time, Hrs, Min),
    format_time(Hrs), write(':'),
    format_time(Min).

not( X ) :- X, !, fail.
not( _ ).

flight_time(Dep, Arv, Time) :-
    airport(Dep, _, degmin(D1,M1), degmin(D2,M2)),
    airport(Arv, _, degmin(D3,M3), degmin(D4,M4)),
    radian_converter(degmin(D1,M1), Lat1),
    radian_converter(degmin(D2,M2), Lon1),
    radian_converter(degmin(D3,M3), Lat2),
    radian_converter(degmin(D4,M4), Lon2),
    haversine(Lat1, Lon1, Lat2, Lon2, Dist),
    Time is Dist/500.

listpath(Node, End, DepTime, ArvTime, Route) :-
    listpath(Node, End, [Node], 0, DepTime, ArvTime, Route).

listpath(Node, Node, _, _, [], [], [Node]).

listpath(Node, End, Visited, CurTime,
    [NextDep|Dep], [NextArv|Arv], [Node|List] ) :-
    flight( Node, Next, time(Hrs, Min)),
    not(member( Next, Visited)),
    NextDep is Hrs + Min/60,
    NextDep >= CurTime + 0.5,
    flight_time(Node, Next, Flight_time),
    NextArv is NextDep + Flight_time,
    listpath(Next, End, [Next|Visited], NextArv, Dep, Arv, List).

writeflight([_|[]],_,_).
writeflight([Dep, Arv|Tail], [DepTime|NextDep], [ArvTime|NextArv]) :-
    airport(Dep, NameDep, _, _),
    airport(Arv, NameArv, _, _), nl,
    write('depart '), write(Dep), write(' '), write(NameDep),
    write(' '), write_time(DepTime), nl,
    write('arrive '), write(Arv), write(' '), write(NameArv),
    write(' '), write_time(ArvTime),
    writeflight([Arv|Tail], NextDep, NextArv).


fly(Dep, Dep) :-
    nl, write('Cannot schedule, Destination is the same as Departure.'), nl,
    !, fail.

fly(Dep, Arv) :-
    airport(Dep, _, _, _),
    airport(Arv, _, _, _),
    listpath(Dep, Arv, DepList, ArvList, RouteList),
    writeflight(RouteList,DepList,ArvList), nl,
    true.

fly(Dep, Arv) :-
    airport(Dep, _, _, _),
    airport(Arv, _, _, _),
    nl, write('No possible route from '), write(Dep),
    write(' to '), write(Arv), nl, !,
    fail.

fly(_,_) :-
    nl, write('Nonexistent airports.'), nl,
   !, fail.


