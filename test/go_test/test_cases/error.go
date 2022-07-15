package test_cases

import (
	"bufio"
	"bytes"
	"fmt"
	"log"
	"net"
	"net/http"
)

// エラーケース
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
			name: "MethodNotImplemented",
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
			expectStatus: http.StatusNotImplemented,
		},
		{
			name: "MethodNotAllowed",
			request: func() *http.Request {
				port := ":1111"
				target := ""
				url := baseURL + port + target
				req, err := http.NewRequest("DELETE", url, nil)
				if err != nil {
					panic(fmt.Errorf("NewRequest: %w", err))
				}
				return req
			}(),
			expectStatus: http.StatusMethodNotAllowed,
		},
		{
			name: "RequestEntityTooLarge",
			request: func() *http.Request {
				port := ":1111"
				target := ""
				url := baseURL + port + target
				body := bytes.NewBufferString(string1001())
				req, err := http.NewRequest("POST", url, body)
				if err != nil {
					panic(fmt.Errorf("NewRequest: %w", err))
				}
				return req
			}(),
			expectStatus: http.StatusRequestEntityTooLarge,
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

func BadRequest() {
	log.Printf("%sRUN BadRequest Test%s", blue, reset)
	defer log.Printf("%sEND BadRequest Test%s", blue, reset)

	conn, err := net.Dial("tcp", "localhost:1111")
	if err != nil {
		log.Println(err)
		return
	}
	defer conn.Close()

	// HostなしGETリクエスト
	body := "GET / HTTP/1.1\r\n\r\n"
	expectResp := "HTTP/1.1 400 Bad Request"

	_, err = fmt.Fprintf(conn, body)
	if err != nil {
		log.Println(err)
		return
	}

	scanner := bufio.NewScanner(conn)
	scanner.Scan()
	actual := scanner.Text()
	if err != nil {
		log.Println(err)
		return
	}

	if actual != expectResp {
		log.Printf(" %s[ERROR]%s\n\texpect: %s\n\tactual: %s",
			red,
			reset,
			expectResp,
			actual,
		)
		return
	}

	log.Println(green, "OK", reset)
}
