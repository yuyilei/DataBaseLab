select name, count(distinct state) num 
from parties 
where name != '' and type = 'Defendant' 
group by name 
order by num desc 
limit 3 offset 0 ;
