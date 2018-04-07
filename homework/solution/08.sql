with tmp_table(now,age) as (select strftime('%Y',cases.filing_date) as now, 
                              strftime('%Y.%m%d',cases.filing_date)-strftime('%Y.%m%d',parties.dob) as age 
                            from ((cases join charges using(case_id)) join parties using (case_id))
                            where cases.filing_date != ''
                                  and parties.dob != '' 
                                  and parties.name != ''
								  and charges.disposition = 'Guilty' 
                                  and parties.type = 'Defendant' 
                                  and age between 0 and 100 )  


select now, avg(age) 
from tmp_table
group by now 
order by now desc 
limit 5 offset 0; 
