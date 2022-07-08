package main

import (
	"webserv_tester/test_cases"
)

func main() {
	test_cases.Get()
	test_cases.Post()
	test_cases.PostChunkedData()
	test_cases.Error()
	test_cases.BadRequest()
}
