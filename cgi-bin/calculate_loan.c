#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcgi_stdio.h>
#include <jansson.h>

json_t* calculate_loan(double loan_amount, double interest_rate, int loan_term) {
  double monthly_interest_rate = interest_rate / 1200.0;
  int num_payments = loan_term * 12;
  double monthly_payment = (loan_amount * monthly_interest_rate) / (1 - pow(1 + monthly_interest_rate, -num_payments));
  double total_interest = (monthly_payment * num_payments) - loan_amount;
  json_t* result = json_object();
  json_object_set_new(result, "monthlyPayment", json_real(monthly_payment));
  json_object_set_new(result, "totalInterest", json_real(total_interest));
  return result;
}

void handle_calculate_loan_request() {
  char* content_length_str = getenv("CONTENT_LENGTH");
  if (content_length_str == NULL) {
    printf("Content-Type: text/plain\r\n\r\n");
    printf("Error: missing request body");
    return;
  }
  int content_length = atoi(content_length_str);
  char* request_body = malloc(content_length + 1);
  fread(request_body, 1, content_length, stdin);
  request_body[content_length] = '\0';
  char loan_amount_str[100], interest_rate_str[100], loan_term_str[100];
  sscanf(request_body, "loanAmount=%s&interestRate=%s&loanTerm=%s", loan_amount_str, interest_rate_str, loan_term_str);
  double loan_amount = atof(loan_amount_str);
  double interest_rate = atof(interest_rate_str);
  int loan_term = atoi(loan_term_str);
  json_t* result = calculate_loan(loan_amount, interest_rate, loan_term);
  char* result_str = json_dumps(result, JSON_INDENT(2));
  printf("Content-Type: application/json\r\n\r\n");
  printf("%s", result_str);
  free(result_str);
  json_decref(result);
  free(request_body);
}

int main() {
  while (FCGI_Accept() >= 0) {
    char* request_method = getenv("REQUEST_METHOD");
    if (request_method != NULL && strcmp(request_method, "POST") == 0) {
      handle_calculate_loan_request();
    } else {
      printf("Content-Type: text/plain\r\n\r\n");
      printf("Error: unsupported request method");
    }
  }
  return 0;
}
