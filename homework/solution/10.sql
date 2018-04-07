select zip, count(case_id) num 
from parties 
where zip != '' 
group by zip 
order by num desc 
limit 3 offset 0;
