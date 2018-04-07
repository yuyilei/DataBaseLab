with tmp_table(case_id,name,disposition) 
	as (select charges.case_id, 
			attorneys.name as name, 
			charges.disposition
		from attorneys join charges using (case_id) 
		where attorneys.name != ''), 

	tmp_table2(name,coun) 
	as (select name, count(name) as coun
			from tmp_table 
			where tmp_table.disposition = 'Not Guilty' 
			group by name ) 

select name, count(tmp_table.case_id) as num, tmp_table2.coun*100.0/count(tmp_table.case_id) as res 
from tmp_table join tmp_table2 using(name) 
group by name
having num > 100 
order by res desc
limit 1 offset 6; 
