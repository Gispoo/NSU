package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strconv"
)

const (
	radiusMeters           = 10000
	maxPlaceDescriptionLen = 200
)

type PlacesResponse struct {
	Count    int     `json:"count"`
	Next     string  `json:"next"`
	Previous *string `json:"previous"`
	Results  []Place `json:"results"`
}

type Place struct {
	ID            int    `json:"id"`
	Title         string `json:"title"`
	Slug          string `json:"slug"`
	Address       string `json:"address"`
	Phone         string `json:"phone"`
	SiteURL       string `json:"site_url"`
	Subway        string `json:"subway"`
	IsClosed      bool   `json:"is_closed"`
	Location      string `json:"location"`
	HasParkingLot bool   `json:"has_parking_lot"`
	Details       string
}

type PlaceDetails struct {
	ID              int    `json:"id"`
	Title           string `json:"title"`
	Slug            string `json:"slug"`
	Address         string `json:"address"`
	Timetable       string `json:"timetable"`
	Phone           string `json:"phone"`
	IsStub          bool   `json:"is_stub"`
	BodyText        string `json:"body_text"`
	Description     string `json:"description"`
	SiteURL         string `json:"site_url"`
	ForeignURL      string `json:"foreign_url"`
	Subway          string `json:"subway"`
	FavoritesCount  int    `json:"favorites_count"`
	CommentsCount   int    `json:"comments_count"`
	IsClosed        bool   `json:"is_closed"`
	Location        string `json:"location"`
	DisableComments bool   `json:"disable_comments"`
	HasParkingLot   bool   `json:"has_parking_lot"`
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func getPlaces(data *Data) {
	data.mutex.Lock()
	if data.selectedLocation < 0 || data.selectedLocation >= maxLocationsNum {
		data.mutex.Unlock()
		return
	}
	lat := strconv.FormatFloat(data.locations[data.selectedLocation].Point.Lat, 'f', -1, 64)
	lon := strconv.FormatFloat(data.locations[data.selectedLocation].Point.Lng, 'f', -1, 64)
	data.mutex.Unlock()

	params := map[string]string{"lon": lon, "lat": lat, "radius": strconv.Itoa(radiusMeters)}
	responseBytes, err := getBytesFromURL(kudagoURL, params)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка HTTP GET запроса:", err)
		return
	}

	var responseDecoded PlacesResponse
	err = json.Unmarshal(responseBytes, &responseDecoded)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка при парсинге JSON:", err)
		return
	}

	if responseDecoded.Count == 0 {
		fmt.Fprintln(os.Stderr, "Интересные места не найдены")
		data.places = make([]Place, 0)
		return
	}

	data.mutex.Lock()
	placesNum := min(responseDecoded.Count, maxPlacesNum)
	data.places = make([]Place, placesNum)
	for i := 0; i < placesNum; i++ {
		data.places[i] = responseDecoded.Results[i]
	}
	data.mutex.Unlock()

	for i := 0; i < placesNum; i++ {
		go getPlaceDetails(data, responseDecoded.Results[i].ID, i)
	}
}

func getPlaceDetails(data *Data, placeID int, placeNum int) {
	params := map[string]string{}
	responseBytes, err := getBytesFromURL(kudagoURL+strconv.Itoa(placeID)+"/", params)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка HTTP GET запроса:", err)
		return
	}

	var responseDecoded PlaceDetails
	err = json.Unmarshal(responseBytes, &responseDecoded)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Ошибка при парсинге JSON:", err)
		return
	}
	description := responseDecoded.Description
	if len(description) > 6 {
		description = description[3 : len(responseDecoded.Description)-3]
	}
	if len(description) > maxPlaceDescriptionLen {
		description = description[:maxPlaceDescriptionLen-3] + "..."
	}

	data.mutex.Lock()
	if placeNum < len(data.places) && data.places[placeNum].ID == placeID {
		data.places[placeNum].Details = description
	}
	data.mutex.Unlock()
}

func placesToText(places []Place) string {
	ans := ""
	for i := 0; i < len(places) && i < maxPlacesNum; i++ {
		if places[i].Title != "" {
			ans += strconv.Itoa(i+1) + ". " + places[i].Title + "\n" + places[i].Details + "\n"
		}
	}
	return ans
}
