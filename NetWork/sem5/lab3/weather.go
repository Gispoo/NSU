package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strconv"
)

type Weather struct {
	Coord      WeatherCoord  `json:"coord"`
	Weather    []WeatherInfo `json:"weather"`
	Base       string        `json:"base"`
	Main       WeatherMain   `json:"main"`
	Visibility int           `json:"visibility"`
	Wind       WeatherWind   `json:"wind"`
	Rain       WeatherRain   `json:"rain"`
	Clouds     WeatherClouds `json:"clouds"`
	Dt         int64         `json:"dt"`
	Sys        WeatherSys    `json:"sys"`
	Timezone   int           `json:"timezone"`
	ID         int           `json:"id"`
	Name       string        `json:"name"`
	Cod        int           `json:"cod"`
}

type WeatherCoord struct {
	Lon float64 `json:"lon"`
	Lat float64 `json:"lat"`
}

type WeatherInfo struct {
	ID          int    `json:"id"`
	Main        string `json:"main"`
	Description string `json:"description"`
	Icon        string `json:"icon"`
}

type WeatherMain struct {
	Temp      float64 `json:"temp"`
	FeelsLike float64 `json:"feels_like"`
	TempMin   float64 `json:"temp_min"`
	TempMax   float64 `json:"temp_max"`
	Pressure  int     `json:"pressure"`
	Humidity  int     `json:"humidity"`
	SeaLevel  int     `json:"sea_level"`
	GrndLevel int     `json:"grnd_level"`
}

type WeatherWind struct {
	Speed float64 `json:"speed"`
	Deg   int     `json:"deg"`
	Gust  float64 `json:"gust"`
}

type WeatherRain struct {
	OneHour float64 `json:"1h"`
}

type WeatherClouds struct {
	All int `json:"all"`
}

type WeatherSys struct {
	Type    int    `json:"type"`
	ID      int    `json:"id"`
	Country string `json:"country"`
	Sunrise int64  `json:"sunrise"`
	Sunset  int64  `json:"sunset"`
}

func getWeather(data *Data) {
	data.mutex.Lock()
	if data.selectedLocation < 0 || data.selectedLocation >= maxLocationsNum {
		data.mutex.Unlock()
		return
	}
	lat := strconv.FormatFloat(data.locations[data.selectedLocation].Point.Lat, 'f', -1, 64)
	lon := strconv.FormatFloat(data.locations[data.selectedLocation].Point.Lng, 'f', -1, 64)
	data.mutex.Unlock()

	params := map[string]string{"lat": lat, "lon": lon, "appid": openweatherKEY}
	responseBytes, err := getBytesFromURL(openweatherURL, params)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка HTTP GET запроса:", err)
		return
	}

	var responseDecoded Weather
	err = json.Unmarshal(responseBytes, &responseDecoded)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка при парсинге JSON:", err)
		return
	}

	data.mutex.Lock()
	data.weather = responseDecoded
	data.mutex.Unlock()
}

func weatherToText(weather Weather) string {
	if weather.Name == "" {
		return ""
	}
	ans := weather.Weather[0].Description
	ans += " " + strconv.FormatFloat(weather.Main.Temp-272.15, 'f', 1, 64) + "℃"
	return ans
}
