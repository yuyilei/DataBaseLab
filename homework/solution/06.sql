select count(status)*100.0/(select count(case_id) from cases) 
from cases 
where status = 'Case Closed Statistically' ;
