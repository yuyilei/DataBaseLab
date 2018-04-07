select count(case_id) num, substr(filing_date,0,4) || '0s' decade 
from cases 
where filing_date !=  '' 
group by decade 
order by num desc 
limit 3 offset 0;
