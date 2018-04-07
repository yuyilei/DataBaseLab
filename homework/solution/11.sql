with tmp_table(case_id,name,disposition) 
		as (select attorneys.case_id as case_id, 
                   attorneys.name as name, 
				   charges.disposition as disposition
			from charges join attorneys using(case_id)
            where attorneys.name != '' ), 

    tmp_table2(name,coun) 
        as (select name, count(case_id) as coun 
            from tmp_table 
            where disposition = 'Not Guilty'  
            group by name ) 


select name, count(tmp_table.case_id) as num, tmp_table2.coun*100.0/count(tmp_table.case_id) as res  
from tmp_table join tmp_table2 using(name) 
group by name 
having num > 100 
order by res desc  
limit 5 offset 0; 



