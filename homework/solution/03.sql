select state, count(case_id) num 
from charges 
where description is not '' and descrption like '%RECKLESS%'  
group by state 
having state is not '' 
order by num desc, state 
limit 3 offset 0;
