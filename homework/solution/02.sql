select count(case_id) 
from charges 
where description like '%PHONE%';
