package test_cases

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
)

const (
	baseURL = "http://127.0.0.1"

	red   = "\033[31m"
	green = "\033[32m"
	blue  = "\033[34m"
	reset = "\033[39m"
)

func Error() {
	log.Printf("%sRUN Error Test%s", blue, reset)
	defer log.Printf("%sEND Error Test%s", blue, reset)
	tests := []struct {
		name         string
		request      *http.Request
		expectStatus int
	}{
		{
			name: "NotFound",
			request: func() *http.Request {
				port := ":1111"
				target := "/no_such_file"
				url := baseURL + port + target
				req, err := http.NewRequest("GET", url, nil)
				if err != nil {
					panic(fmt.Errorf("NewRequest: %w", err))
				}
				return req
			}(),
			expectStatus: http.StatusNotFound,
		},
		{
			name: "MethodNotAllowed",
			request: func() *http.Request {
				port := ":1111"
				target := ""
				url := baseURL + port + target
				req, err := http.NewRequest("HOGE", url, nil)
				if err != nil {
					panic(fmt.Errorf("NewRequest: %w", err))
				}
				return req
			}(),
			expectStatus: http.StatusMethodNotAllowed,
		},
	}

	client := http.DefaultClient
	for _, tt := range tests {
		log.Println(" Run:", tt.name)

		resp, err := client.Do(tt.request)
		if err != nil {
			log.Println(red, " [ERROR]", reset, err)
			continue
		}
		defer resp.Body.Close()

		if resp.StatusCode != tt.expectStatus {
			log.Printf(" %s[ERROR]%s status code\n\texpect: %d\n\tactual: %d",
				red,
				reset,
				tt.expectStatus,
				resp.StatusCode)
			continue
		}
		log.Println(green, "OK", reset)
	}
}

func Get() {
	log.Printf("%sRUN Get Test%s", blue, reset)
	defer log.Printf("%sEND Get Test%s", blue, reset)
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
