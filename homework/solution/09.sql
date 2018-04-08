/*
 原答案：查询的表弄错了，题意也没弄清楚。。。 
 */
with tmp_table(race,disposition) 
			as (select parties.race as race,
					cases.disposition as disposition
                from cases join parties using(case_id) 
                where parties.race in ('African American','Caucasian') 
                    and cases.disposition in ('Guilty','Not Guilty')), 

    tmp_table2(race,coun) 
            as (select race, count(race) 
                from tmp_table 
                group by race) 


select tmp_table.race, tmp_table.disposition, 
        count(tmp_table.race)*100.0/tmp_table2.coun as per 
from tmp_table join tmp_table2 using(race) 
group by tmp_table.race, tmp_table.disposition; 

/*
订正后的答案。。。 
*/
with tmp_table(race,disposition) 
			as (select parties.race as race,
					charges.disposition as disposition
                from charges join parties using(case_id) 
                where parties.race in ('African American','Caucasian') 
                    and charges.disposition in ('Guilty','Not Guilty')), 

    tmp_table2(disposition,coun) 
            as (select disposition, count(disposition) 
                from tmp_table 
                group by disposition) 


select tmp_table.race, tmp_table.disposition, 
        count(tmp_table.race)*100.0/tmp_table2.coun as per 
from tmp_table join tmp_table2 using(disposition) 
group by tmp_table.race, tmp_table.disposition; 
