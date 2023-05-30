CREATE TABLE IF NOT EXISTS teachers ( 
  t_id bigint PRIMARY KEY NOT NULL,
  t_name varchar(500) NOT NULL,
  t_sex varchar(500),
  t_contact varchar(500),
  t_address varchar(500),
  t_dept varchar(500)
);

INSERT INTO teachers values (1100000001, 'Dominic Guiritan', 'Male', '09465548465', 'Butuan City, Agusan del Norte', 'CSP'), (1100000002, 'Daj Bonggo', 'Male', '09146616565', 'Butuan City, Agusan del Norte', 'CSP'), (1100000003, 'Shiela Luceno', 'Female', '09616656253', 'Butuan City, Agusan del Norte', 'CSP'), (1100000004, 'Rejeenald Flores', 'Male', '09216541655', 'Buenavista, Agusan del Norte', 'CSP'), (1100000005, 'Lamberto Boligor', 'Male', '09664568962', 'Butuan City, Agusan del Norte', 'CSP');

-- Optional: View the inserted data
SELECT * FROM teachers;