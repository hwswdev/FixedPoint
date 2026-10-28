clear; clc; close all;

% =========================================================================
% 1. НАСТРОЙКИ И ЗАГРУЗКА ДАННЫХ
% =========================================================================
filename = 'ScopeData.csv';
sampling_period = 1e-6; % Шаг дискретизации 1 мкс (из заголовка SP)
fs = 1 / sampling_period; % Частота дискретизации (1 МГц)

% Чтение матрицы данных (пропускаем 8 строк текстового заголовка)
matrix_data = dlmread(filename, ',', 8, 0);

% Извлечение столбцов с учетом сдвига из-за первой запятой в файле
raw_index = matrix_data(:, 2);
raw_ch1   = matrix_data(:, 3);
raw_ch2   = matrix_data(:, 4);

% Перевод в физические величины (Секунды и Вольты)
t = (raw_index - 1) * sampling_period;
ch1 = raw_ch1 / 1000;
ch2 = raw_ch2 / 1000;
N = length(t); % Количество точек

% =========================================================================
% ФИГУРА 1: ВРЕМЕННЫЕ СИГНАЛЫ (ОСЦИЛЛОГРАММА)
% =========================================================================
figure(1);
plot(t, ch1, 'r', 'LineWidth', 1.5); hold on;
plot(t, ch2, 'b', 'LineWidth', 1.5);
grid on;
xlabel('Time, s');
ylabel('Voltage, V');
title('Time Domain Signals (Oscillogram)');
legend('CH1', 'CH2');

% =========================================================================
% ФИГУРА 2: ФИГУРА ЛИССАЖУ
% =========================================================================
figure(2);
plot(ch1, ch2, 'g', 'LineWidth', 1.5);
grid on;
xlabel('CH1 Voltage, V');
ylabel('CH2 Voltage, V');
title('Lissajous Figure (CH1 vs CH2)');
axis equal; % Сохраняем пропорции осей для правильного отображения окружностей/эллипсов

% =========================================================================
% РАСЧЕТ БЫСТРОГО ФУРЬЕ-АНАЛИЗА (FFT)
% =========================================================================
% Вычисляем FFT для обоих каналов
fft_ch1 = fft(ch1) / N;
fft_ch2 = fft(ch2) / N;

% Создаем вектор частот для одностороннего спектра (от 0 до Nyquist)
frequencies = (0:(N/2)) * (fs / N);

% Спектр амплитуд (умножаем на 2 для сохранения энергии после отсечения зеркальной половины)
amp_ch1 = 2 * abs(fft_ch1(1:floor(N/2)+1));
amp_ch2 = 2 * abs(fft_ch2(1:floor(N/2)+1));

% Спектр фаз (в градусах)
% Используем порог по амплитуде, чтобы убрать случайную фазу фонового шума
threshold = 0.01; % Игнорировать фазу для гармоник с амплитудой меньше 10 мВ
phase_ch1 = angle(fft_ch1(1:floor(N/2)+1)) * (180 / pi);
phase_ch2 = angle(fft_ch2(1:floor(N/2)+1)) * (180 / pi);
phase_ch1(amp_ch1 < threshold) = 0;
phase_ch2(amp_ch2 < threshold) = 0;

% =========================================================================
% ФИГУРА 3: СПЕКТРЫ АМПЛИТУД И ФАЗ (FFT)
% =========================================================================
figure(3);

% Верхний подграфик: Амплитудный спектр
subplot(2, 1, 1);
stem(frequencies, amp_ch1, 'r', 'LineWidth', 1.2, 'Marker', 'none'); hold on;
stem(frequencies, amp_ch2, 'b', 'LineWidth', 1.2, 'Marker', 'none');
grid on;
xlabel('Frequency, Hz');
ylabel('Amplitude, V');
title('FFT Amplitude Spectrum');
legend('CH1 Spectrum', 'CH2 Spectrum');

% Ниже подграфик: Фазовый спектр
subplot(2, 1, 2);
stem(frequencies, phase_ch1, 'r', 'LineWidth', 1.2); hold on;
stem(frequencies, phase_ch2, 'b', 'LineWidth', 1.2);
grid on;
xlabel('Frequency, Hz');
ylabel('Phase, Degrees');
title('FFT Phase Spectrum');
ylim([-180, 180]);

