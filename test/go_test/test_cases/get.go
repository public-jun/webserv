package test_cases

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
)

func Get() {
	log.Printf("%sRUN Get Test%s", blue, reset)
	defer log.Printf("%sEND Get Test%s", blue, reset)

	indexHTML, err := ioutil.ReadFile("../../docs/html/index.html")
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
			name: "Index,NoTarget",
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
			name: "AutoIndexON,NoTarget",
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
			name: "NoIndex,NoTarget",
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
		log.Println(" RUN:", tt.name)

		resp, err := client.Do(tt.request)
		if err != nil {
			log.Println(red, "[ERROR]", reset, err)
			continue
		}
		defer resp.Body.Close()

		if resp.StatusCode != tt.expectStatus {
			log.Printf("%s[ERROR]%s status code\n\texpect: %d\n\tactual: %d",
				red,
				reset,
				tt.expectStatus,
				resp.StatusCode)
			continue
		}

		if resp.StatusCode == http.StatusOK {
			body, err := ioutil.ReadAll(resp.Body)
			if err != nil {
				log.Println("ReadAll:", err)
				continue
			}

			if tt.expectIndex {
				if string(body) != string(indexHTML) {
					log.Println(red, "[ERROR]", reset, "body")
					continue
				}
			} else {
				if string(body) == string(indexHTML) {
					log.Println(red, "[ERROR]", reset, "body")
					continue
				}
			}
		}

		log.Println(green, "OK", reset)
	}
}
