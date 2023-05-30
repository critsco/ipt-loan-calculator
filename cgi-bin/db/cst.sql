CREATE TABLE IF NOT EXISTS students ( 
  s_id bigint PRIMARY KEY NOT NULL,
  s_name varchar(500) NOT NULL,
  s_sex varchar(500),
  s_contact varchar(500),
  s_address varchar(500),
  s_dept varchar(500),
  s_course varchar(500),
  s_year integer,
  s_section varchar(500)
);

INSERT INTO students values (21100001011, 'Oliver Wendell Ceniza', 'Male', '09672599956', 'P-Narra, Talisay, Nasipit, Agusan del Norte', 'CSP', 'Diploma in Information Technology', 2, 'DPIT21');


-- Optional: View the inserted data
SELECT * FROM students;
