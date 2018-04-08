select name, count(distinct violation_county) num 
from parties join cases using(case_id)  
where name != '' and parties.type = 'Defendant' 
group by name 
order by num desc 
limit 3 offset 0 ;
