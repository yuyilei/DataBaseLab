select parties.zip, count(parties.case_id) num 
from parties join charges using(case_id)  
where parties.zip != '' 
group by parties.zip 
order by num desc 
limit 3 offset 0;
