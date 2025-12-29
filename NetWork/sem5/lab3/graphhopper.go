package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strconv"
)

type Location struct {
	Point       GeohopperPoint `json:"point"`
	Extent      []float64      `json:"extent,omitempty"`
	Name        string         `json:"name"`
	Country     string         `json:"country"`
	City        string         `json:"city"`
	CountryCode string         `json:"countrycode"`
	State       string         `json:"state,omitempty"`
	Street      string         `json:"street"`
	Postcode    string         `json:"postcode,omitempty"`
	OsmID       int64          `json:"osm_id"`
	OsmType     string         `json:"osm_type"`
	OsmKey      string         `json:"osm_key"`
	OsmValue    string         `json:"osm_value"`
}

type GeohopperPoint struct {
	Lat float64 `json:"lat"`
	Lng float64 `json:"lng"`
}

type GeohopperResponse struct {
	Hits   []Location `json:"hits"`
	Locale string     `json:"locale"`
}

func getLocations(description string, data *Data) {
	params := map[string]string{"q": description, "limit": strconv.Itoa(maxLocationsNum), "key": graphhopperKEY}
	responseBytes, err := getBytesFromURL(graphhopperURL, params)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка HTTP GET запроса:", err)
		return
	}

	var responseDecoded GeohopperResponse
	err = json.Unmarshal(responseBytes, &responseDecoded)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка при парсинге JSON:", err)
		return
	}

	if len(responseDecoded.Hits) == 0 {
		fmt.Fprintln(os.Stderr, "Нет результатов по данному описанию локации")
		return
	}

	data.mutex.Lock()
	copy(data.locations, responseDecoded.Hits)
	data.selectedLocation = -1
	data.mutex.Unlock()
}

func getLocationText(location Location) string {
	ans := location.Name
	if location.Country != "" {
		ans += ", страна: " + location.Country
	}
	if location.City != "" {
		ans += ", город: " + location.City
	}
	if location.State != "" {
		ans += ", субъект: " + location.State
	}
	if location.Street != "" {
		ans += ", улица: " + location.Street
	}
	ans += ", (" + strconv.FormatFloat(location.Point.Lat, 'f', -1, 64) + ", " + strconv.FormatFloat(location.Point.Lng, 'f', -1, 64) + ")"
	return ans
}
