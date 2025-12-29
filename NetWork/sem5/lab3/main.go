package main

import (
	"log"
	"os"
	"strings"
	"sync"

	"image/color"

	"gioui.org/app"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/text"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
)

const (
	maxLocationsNum              = 5
	maxPlacesNum                 = 5
	defaultInputLocationNameText = "Введите название локации..."
	graphhopperURL               = "https://graphhopper.com/api/1/geocode"
	graphhopperKEY               = "34e2aa3d-458f-4dec-978c-8f4045d876b6"
	openweatherURL               = "https://api.openweathermap.org/data/2.5/weather"
	openweatherKEY               = "688ffe0da9e16360228d1cb034013087"
	kudagoURL                    = "https://kudago.com/public-api/v1.2/places/"
)

type Data struct {
	locations        []Location
	selectedLocation int // Номер локации в locations, уоторая сейчас выбрана
	weather          Weather
	places           []Place
	mutex            sync.Mutex
}

func (data *Data) getSelectedLocationName() string {
	if data.selectedLocation != -1 {
		return data.locations[data.selectedLocation].Name
	} else {
		return ""
	}
}

func main() {
	go run()
	app.Main()
}

func run() {
	window := new(app.Window)
	window.Option(app.Title("Places Finder"))
	window.Option(app.Size(unit.Dp(600), unit.Dp(800)))

	err := loop(window)
	if err != nil {
		log.Fatal(err)
	} else {
		os.Exit(0)
	}
}

// Логика работы (обработка событий + отрисовка)
func loop(window *app.Window) error {
	theme := material.NewTheme()
	theme.Palette.ContrastBg = color.NRGBA{R: 51, G: 173, B: 255, A: 255}

	// Контекст операций рисования ("журнал" команд, которые описывают, как элементы должны быть отрисованы в окне)
	var ops op.Ops

	// Виджет для ввода текста с клавиатуры:
	locationNameInput := new(widget.Editor)
	locationNameInput.Submit = true
	locationNameInput.SingleLine = true

	// Виджеты для выбора локации из списка
	locationsList := new(widget.List)
	locationsList.Axis = layout.Vertical
	var locationButtons [maxLocationsNum]*widget.Clickable
	for i := 0; i < maxLocationsNum; i++ {
		locationButtons[i] = new(widget.Clickable)
	}

	data := Data{locations: make([]Location, maxLocationsNum), selectedLocation: -1}

	for {
		switch event := window.Event().(type) {
		case app.DestroyEvent:
			return event.Err

		case app.FrameEvent:
			gtx := app.NewContext(&ops, event)

			drawInterface(gtx, theme, locationNameInput, locationsList, locationButtons, &data)

			event.Frame(gtx.Ops)
		}
	}
}

func drawInterface(gtx layout.Context, theme *material.Theme,
	locationNameInput *widget.Editor,
	locationsList *widget.List,
	locationButtons [maxLocationsNum]*widget.Clickable,
	data *Data) {

	inset := layout.UniformInset(unit.Dp(15))
	inset.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
		return drawVerticalLayout(gtx, theme, locationNameInput, locationsList, locationButtons, data)
	})
}

func drawVerticalLayout(gtx layout.Context, theme *material.Theme,
	locationNameInput *widget.Editor,
	locationsList *widget.List,
	locationButtons [maxLocationsNum]*widget.Clickable,
	data *Data) layout.Dimensions {

	return layout.Flex{Axis: layout.Vertical}.Layout(gtx,
		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return drawInputField(gtx, theme, locationNameInput, data)
		}),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return drawLocationsList(gtx, theme, locationsList, locationButtons, data)
		}),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return drawSpacer(gtx)
		}),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return drawInfoText(gtx, theme, data)
		}),
	)
}

func drawInputField(gtx layout.Context, theme *material.Theme,
	locationNameInput *widget.Editor, data *Data) layout.Dimensions {

	// Обработка ввода текста
	inputEvent, _ := locationNameInput.Update(gtx)
	if e, ok := inputEvent.(widget.SubmitEvent); ok {
		go getLocations(e.Text, data)
		locationNameInput.SetText("")
	}

	// Отрисовка поля ввода
	editor := material.Editor(theme, locationNameInput, defaultInputLocationNameText)
	return editor.Layout(gtx)
}

// Список локаций
func drawLocationsList(gtx layout.Context, theme *material.Theme,
	locationsList *widget.List,
	locationButtons [maxLocationsNum]*widget.Clickable,
	data *Data) layout.Dimensions {

	return material.List(theme, locationsList).Layout(gtx, maxLocationsNum, func(gtx layout.Context, i int) layout.Dimensions {
		return drawLocationItem(gtx, theme, locationButtons[i], data, i)
	})
}

// Один элемент списка локаций
func drawLocationItem(gtx layout.Context, theme *material.Theme,
	button *widget.Clickable, data *Data, index int) layout.Dimensions {

	data.mutex.Lock()
	locationName := data.locations[index].Name
	buttonText := getLocationText(data.locations[index])
	data.mutex.Unlock()

	if locationName == "" {
		// Пустая кнопка если локации нет
		btn := material.Button(theme, button, "")
		return btn.Layout(gtx)
	}

	// Кнопка с текстом локации
	btn := material.Button(theme, button, buttonText)

	// Обработка клика
	if button.Clicked(gtx) {
		data.selectedLocation = index
		go getWeather(data)
		go getPlaces(data)
	}

	return btn.Layout(gtx)
}

// Разделитель
func drawSpacer(gtx layout.Context) layout.Dimensions {
	spacer := layout.Spacer{Height: unit.Dp(10)}
	return spacer.Layout(gtx)
}

// Текстовая информация
func drawInfoText(gtx layout.Context, theme *material.Theme, data *Data) layout.Dimensions {
	fullText := buildInfoText(data)

	title := material.H6(theme, fullText)
	title.Alignment = text.Start
	return title.Layout(gtx)
}

// Формирование текста информации
func buildInfoText(data *Data) string {
	data.mutex.Lock()
	defer data.mutex.Unlock()

	var text strings.Builder
	text.WriteString("Локация: " + data.getSelectedLocationName() + "\n")
	text.WriteString("Погода: " + weatherToText(data.weather) + "\n")
	text.WriteString("Места: \n" + placesToText(data.places) + "\n")

	return text.String()
}

// какие есть запросы (get, post, put)? в чем их отличие?
// разобраться как работает http (подробно)
// как устроена асинхроность?
