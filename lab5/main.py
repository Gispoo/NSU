import cv2
import time

# Инициализация камеры
cap = cv2.VideoCapture(0)

# Проверка успешности открытия камеры
if not cap.isOpened():
  raise IOError("Не удалось открыть камеру!")

# Задание преобразования (например, поворот на 90 градусов)
def transform_image(frame):
  return cv2.rotate(frame, cv2.ROTATE_90_CLOCKWISE)

# Обработка видеопотока
fps_start_time = time.time()
frame_count = 0
while(True):
  ret, frame = cap.read()
  if not ret:
    break

  # Преобразование изображения
  transformed_frame = transform_image(frame)

  # Показ преобразованного кадра
  cv2.imshow('Video', transformed_frame)

  # Подсчет кадров в секунду
  frame_count += 1
  if (time.time() - fps_start_time) > 1:
    fps = frame_count / (time.time() - fps_start_time)
    fps_start_time = time.time()
    frame_count = 0
    print(f"FPS: {fps}")

  # Выход из цикла при нажатии клавиши 'q'
  if cv2.waitKey(1) & 0xFF == ord('q'):
    break

# Закрытие окна и освобождение ресурсов
cap.release()
cv2.destroyAllWindows()
