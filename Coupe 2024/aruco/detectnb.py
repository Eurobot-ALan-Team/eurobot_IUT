import numpy as np
import cv2
import cv2.aruco as aruco

marker_size = 100 
with open('camera_cal.npy', 'rb') as f:
	camera_matrix = np.load(f)
	camera_distortion = np.load(f)

aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_4X4_250)

cap = cv2.VideoCapture(0)

camera_width = 640
camera_height = 480
camera_frame_rate = 40 

cap.set(2, camera_width)
cap.set(4, camera_height)
cap.set(5, camera_frame_rate)

while True:

	ret, frame = cap.read()

	gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

	corners, ids, rejected = aruco.detectMarkers(gray_frame, aruco_dict, camera_matrix, camera_distortion)

	if ids is not None:

		aruco.drawDetectedMarkers(frame, corners)

		rvec, tvec, _objPoints = aruco.estimatePoseSingleMarkers(corners, marker_size, camera_matrix, camera_distortion)

		for marker in range(len(ids)):
			aruco.drawAxis(frame, camera_matrix, camera_distortion, rvec[marker], tvec[marker], 100)
			cv2.putText(frame, str(ids[marker][0]) ,( int(corners[marker][0][0][0]) - 30, int(corners[marker][0][0][1])), cv2.FONT_HERSHEY_PLAIN, 3, (255, 0, 0), 2, cv2.LINE_AA)

	cv2.imshow('frame', frame)

	key = cv2.waitKey(1) & 0xFF
	if key == ord('q'): break
	

cap.release()
cv2.destroyAllWindows()