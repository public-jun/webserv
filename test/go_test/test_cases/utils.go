package test_cases

const (
	baseURL = "http://127.0.0.1"

	red   = "\033[31m"
	green = "\033[32m"
	blue  = "\033[34m"
	reset = "\033[39m"
)

// 長さ1001の文字列生成
func string1001() string {
	var s string
	for i := 0; i < 1001; i++ {
		s += "a"
	}
	return s
}
