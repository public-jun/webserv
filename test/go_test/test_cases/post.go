package test_cases

import (
	"io"
	"log"
	"net/http"
	"net/url"
	"os"
	"strings"
)

func Post() {
	log.Printf("%sRUN Post Test%s", blue, reset)
	defer log.Printf("%sEND Post Test%s", blue, reset)

	resp, err := http.Post("http://localhost:1111", "text/plain", strings.NewReader("hoge"))
	if err != nil {
		log.Println(err)
		return
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusCreated {
		log.Printf(" %s[ERROR]%s status code\n\texpect: %d\n\tactual: %d",
			red,
			reset,
			http.StatusCreated,
			resp.StatusCode,
		)
		return
	}

	log.Println(green, "OK", reset)
}

func PostChunkedData() {
	log.Printf("%sRUN PostChunkedData Test%s", blue, reset)
	defer log.Printf("%sEND PostChunkedData Test%s", blue, reset)

	url, err := url.Parse("http://localhost:1111")
	if err != nil {
		log.Println(err)
		return
	}

	reader, writer := io.Pipe()

	req := &http.Request{
		Method:           "POST",
		URL:              url,
		ProtoMajor:       1,
		ProtoMinor:       1,
		TransferEncoding: []string{"chunked"},
		Body:             reader,
		Header:           make(map[string][]string),
	}

	client := http.DefaultClient

	f, err := os.Open("./test_cases/chunked_data.txt")
	if err != nil {
		log.Println(err)
		return
	}

	go func() {
		for {
			buf := make([]byte, 3)
			n, err := f.Read(buf)
			if n == 0 {
				break
			}
			if err != nil {
				panic(err)
			}
			writer.Write(buf)
		}
		err = writer.Close()
		if err != nil {
			panic(err)
		}
	}()

	resp, err := client.Do(req)
	if err != nil {
		log.Println(err)
		return
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusCreated {
		log.Printf(" %s[ERROR]%s status code\n\texpect: %d\n\tactual: %d",
			red,
			reset,
			http.StatusCreated,
			resp.StatusCode,
		)
		return
	}

	log.Println(green, "OK", reset)
}
