select violation_county, count(case_id) num 
from charges join cases using(case_id)  
where description is not '' and description like '%RECKLESS%'  
group by violation_county 
having violation_county is not '' 
order by num desc, violation_county 
limit 3 offset 0;
