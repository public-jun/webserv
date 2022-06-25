package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
)

const baseURL = "http://127.0.0.1"

func testGet() {
	indexHTML, err := ioutil.ReadFile("../../index.html")
	if err != nil {
		panic(err)
	}

	tests := []struct {
		name         string
		request      *http.Request
		expectStatus int
		expectIndex  bool
	}{
		{
			name: "ExistIndex,NotExistTarget",
			request: func() *http.Request {
				port := ":1111"
				target := ""
				url := baseURL + port + target
				req, err := http.NewRequest("GET", url, nil)
				if err != nil {
					panic(fmt.Errorf("NewRequest: %w", err))
				}
				return req
			}(),
			expectStatus: http.StatusOK,
			expectIndex:  true,
		},
		{
			name: "ExistAutoIndex,NotExistTarget",
			request: func() *http.Request {
				port := ":1112"
				target := ""
				url := baseURL + port + target
				req, err := http.NewRequest("GET", url, nil)
				if err != nil {
					panic(fmt.Errorf("NewRequest: %w", err))
				}
				return req
			}(),
			expectStatus: http.StatusOK,
			expectIndex:  false,
		},
		{
			name: "NotExistIndex,NotExistTarget",
			request: func() *http.Request {
				port := ":1113"
				target := ""
				url := baseURL + port + target
				req, err := http.NewRequest("GET", url, nil)
				if err != nil {
					panic(fmt.Errorf("NewRequest: %w", err))
				}
				return req
			}(),
			expectStatus: http.StatusNotFound,
			expectIndex:  false,
		},
	}

	client := http.DefaultClient
	for _, tt := range tests {
		log.Println("RUN:", tt.name)

		resp, err := client.Do(tt.request)
		if err != nil {
			log.Println("[ERROR]", err)
			continue
		}

		if resp.StatusCode != tt.expectStatus {
			log.Printf("[ERROR] status code\nexpect: %d\nactual: %d",
				tt.expectStatus,
				resp.StatusCode)
			continue
		}

		if resp.StatusCode == http.StatusOK {
			body, err := ioutil.ReadAll(resp.Body)
			if err != nil {
				log.Println("ReadAll:", err)
			}

			if tt.expectIndex {
				if string(body) != string(indexHTML) {
					log.Println("[ERROR] body")
					continue
				}
			} else {
				if string(body) == string(indexHTML) {
					log.Println("[ERROR] body")
					continue
				}
			}
		}

		log.Println("OK")
	}
}

func main() {
	testGet()
}
