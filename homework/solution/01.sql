select count(distinct lower(name)) i
from attorneys 
where lower(name) != 'null' ;
