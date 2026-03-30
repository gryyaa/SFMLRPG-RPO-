#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

const sf::Color ROSY_GRANITE_COLOR = { 163, 149, 148 };
const sf::Color EGGSHELL_COLOR = { 237, 235, 215 };
const sf::Color CHARCOAL_BROWN_COLOR = { 66, 62, 55 };
const sf::Color SUNFLOWER_GOLD_COLOR = { 227, 178, 60 };

// ---------------Структура книги---------------
struct Book
{
	sf::String title;
	sf::String author;
	sf::String year;
};
// ---------------Класс поля ввода---------------
class InputField // shape, событие нажатия по фигуре, отрисовка букв, положение, размер, get-функция по возвращению значения объекта, подпись к полю ввода
{
public:
	// Удобно разграничить функционал конструктора следующим образом: инициализацию производить в значениях по умолчанию, а настраивать объекты в теле конструктора
	//			шрифт,				 подпись к полю ввода,		позиция в окне,		размер
	InputField(const sf::Font& font, sf::String caption, sf::String placeholder, sf::Vector2f position, sf::Vector2f size) : captionText(font, caption, 18), valueText(font, placeholder, 18), placeholderText(placeholder)
	{
		// fieldShape
		fieldShape.setPosition(position);
		fieldShape.setSize(size);
		fieldShape.setFillColor(sf::Color(EGGSHELL_COLOR));
		fieldShape.setOutlineColor(sf::Color(CHARCOAL_BROWN_COLOR));
		fieldShape.setOutlineThickness(3.0f);
		// caption
		captionText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
		captionText.setPosition({ position.x, position.y - 25.0f });
		// valueText
		valueText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
		valueText.setPosition({ position.x + 10.0f, position.y + 5.0f});
	}

	// Функция для проверки попадания в окно
	// Если точка, передаваемая в качестве параметра лежит в границах окна, возвращаем true
	bool Contains(sf::Vector2f point) const
	{
		const sf::Vector2f position = fieldShape.getPosition();
		const sf::Vector2f size = fieldShape.getSize();

		return point.x >= position.x && point.x <= position.x + size.x && point.y >= position.y && point.y <= position.y + size.y;
	}

	// Функция изменения цвета фигуры при активном или неактивном режиме 
	void SetActive(bool isActive)
	{
		this->isActive = isActive;
		fieldShape.setOutlineColor(isActive ? sf::Color(66, 62, 55, 255) : sf::Color(66, 62, 55, 150));
	}

	// Get-функция по возвращению строкового значения поля ввода
	const sf::String& GetValue() const
	{
		return valueString;
	}

	// Вспомогательная функция очистки текста в поле ввода
	void ClearInputValue()
	{
		valueString.clear();
	}

	// Функция-обработчик события введения символа пользователем, принимающая символ юникода
	// char_32_t - тип данных, захватывающий в два больше символов ASCII, где располагается кириллица
	void HandleTextEntered(char32_t unicode)
	{
		if (!isActive) return; // Если пользователь не в активном окне ввода, то завершаем работу функции
		// Ситуации стирания
		if (unicode == 8) // Backspace
		{
			if (!valueString.isEmpty()) valueString.erase(valueString.getSize() - 1);
		}
		else if (unicode >= 32) // Ситуация добавления символов
		{
			if (valueString.getSize() < maxLength) valueString += sf::String(unicode);
		}
	}

	// Функция отрисовки, принимающая ссылку на графическое окно
	void Draw(sf::RenderWindow& renderWindow)
	{
		if (valueString.isEmpty())
		{
			valueText.setString(placeholderText);
			valueText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR.r, CHARCOAL_BROWN_COLOR.g, CHARCOAL_BROWN_COLOR.b, 130));
		}
		else
		{
			valueText.setString(valueString);
			valueText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
		}
		renderWindow.draw(captionText);
		renderWindow.draw(fieldShape);
		renderWindow.draw(valueText);
	}

private:
	// Отрисовываемые элементы
	sf::RectangleShape fieldShape;
	sf::Text captionText; // подпись
	sf::Text valueText;
	// Прочие элементы
	sf::String valueString;
	sf::String placeholderText;
	bool isActive = false;
	unsigned int maxLength = 32;
};
// ---------------Класс кнопки---------------
class Button
{
public:
	Button(const sf::Font& font, sf::String label, sf::Vector2f position, sf::Vector2f size) : label(font, label, 18)
	{
		buttonShape.setPosition(position);
		buttonShape.setSize(size);
		buttonShape.setFillColor(sf::Color(EGGSHELL_COLOR));
		buttonShape.setOutlineColor(CHARCOAL_BROWN_COLOR);
		buttonShape.setOutlineThickness(3.0f);

		this->label.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
		this->label.setPosition({ position.x + 10.0f, position.y + 5.0f });
	}

	bool Contains(sf::Vector2f point) const
	{
		const sf::Vector2f position = buttonShape.getPosition();
		const sf::Vector2f size = buttonShape.getSize();

		return point.x >= position.x && point.x <= position.x + size.x && point.y >= position.y && point.y <= position.y + size.y;
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(buttonShape);
		window.draw(label);
	}

private:
	sf::RectangleShape buttonShape;
	sf::Text label;	
};

int main()
{
	// ---------------Инициализация основного окна---------------
	sf::Font font("Comic Sans MS.ttf");	

	// ---------------Инициализация основного окна---------------
	const unsigned int WINDOW_WIDTH = 800;
	const unsigned int WINDOW_HEIGHT = 600;
	sf::RenderWindow mainWindow(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), L"Домашняя библиотека");
	// Добавление иконки
	sf::Image icon("icon.png");
	mainWindow.setIcon(icon);

	// ---------------Создание объектов---------------
	sf::RectangleShape leftPanel({ 350.0f, 565.0f });
	leftPanel.setPosition({ 15.0f, 15.0f });
	leftPanel.setFillColor(sf::Color(ROSY_GRANITE_COLOR));
	leftPanel.setOutlineColor(sf::Color(CHARCOAL_BROWN_COLOR));
	leftPanel.setOutlineThickness(5.0f);

	sf::RectangleShape rightPanel({ 350.0f, 565.0f });
	rightPanel.setPosition({ 430.0f, 15.0f });
	rightPanel.setFillColor(sf::Color(ROSY_GRANITE_COLOR));
	rightPanel.setOutlineColor(sf::Color(CHARCOAL_BROWN_COLOR));
	rightPanel.setOutlineThickness(5.0f);

	sf::Text statusText(font, "", 18);
	statusText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
	statusText.setPosition({ 90.0f, 540.0f });
	sf::Text instructionText(font, L"Добавьте книгу: заполните поля и нажмите кнопку", 18);
	instructionText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
	instructionText.setPosition({ 20.0f, 40.0f });
	// Объекты для статуса добавления книги
	sf::String statusMessage;
	bool showStatus = false;
	sf::Clock statusClock;

	auto SetStatus = [&](sf::String& message)
		{
			statusMessage = message;
			showStatus = true;
			statusClock.restart();
		};

	std::vector<Book> booksArray;

	// Размер по x - количество букв * 10
													// Размер по y - размер символа * 2
	InputField bookTitleField(font, L"Название книги", L"Введите название книги", { 20.0f, 100.0f }, { 320.0f, 36.f });
	InputField authorField(font, L"Автор книги", L"Введите автора", { 20.0f, 200.0f }, { 320.0f, 36.f });
	InputField yearField(font, L"Год издания", L"Введите год издания", { 20.0f, 300.0f }, { 320.0f, 36.f });

	auto AddBook = [&]()
		{
			if (bookTitleField.GetValue().isEmpty() || authorField.GetValue().isEmpty() || yearField.GetValue().isEmpty())
			{
				sf::String message(L"Не все поля заполнены!");
				SetStatus(message);
				return;
			}

			booksArray.push_back(Book{ bookTitleField.GetValue(), authorField.GetValue(), yearField.GetValue() });

			bookTitleField.ClearInputValue();
			yearField.ClearInputValue();
			authorField.ClearInputValue();
			sf::String message(L"Книга успешно добавлена!");
			SetStatus(message);
		};													

	Button addBookButton(font, L"Добавить книгу", { 90.0f, 500.0f }, { 200.0f, 26.0f });

	// ---------------Обработка событий---------------
	while (mainWindow.isOpen())
	{
		// Обработка события по взаимодействию с главным окном
		while (const std::optional event = mainWindow.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				mainWindow.close();
			}
			// дополнительная обработка события нахождения мыши в пределах окна и нажатия. getIF - вспомогательная функция, определяющая наличие события с дополнительным уссловием
			else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseEvent->button == sf::Mouse::Button::Left)
				{
					// Создание объекта вектора, показывающего координаты нажатия курсора мыши
					sf::Vector2f mousePoint = { float(mouseEvent->position.x), float(mouseEvent->position.y) };

					// Если клик мыши находится в каком-либо из графических объектов на экране, активируем эти объекты
					if (bookTitleField.Contains(mousePoint))
					{
						bookTitleField.SetActive(true);
						authorField.SetActive(false);
						yearField.SetActive(false);
					}
					else if (authorField.Contains(mousePoint))
					{
						authorField.SetActive(true);
						bookTitleField.SetActive(false);
						yearField.SetActive(false);
					}
					else if (yearField.Contains(mousePoint))
					{
						yearField.SetActive(true);
						bookTitleField.SetActive(false);
						authorField.SetActive(false);
					}
					else
					{
						bookTitleField.SetActive(false);
						authorField.SetActive(false);
						yearField.SetActive(false);
					}
					// Обработка нажатия на кнопку
					if (addBookButton.Contains(mousePoint))
					{
						AddBook();
					}
				}
			}
			// Обработка события печатания
			else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
			{
				bookTitleField.HandleTextEntered(textEntered->unicode);
				authorField.HandleTextEntered(textEntered->unicode);
				yearField.HandleTextEntered(textEntered->unicode);
			}
		}
		// Очистка текста статуса
		if (showStatus && statusClock.getElapsedTime().asSeconds() > 2.0f) showStatus = false;
		
		// ---------------Очистка элементов---------------
		mainWindow.clear(sf::Color(227, 178, 60));

		// ---------------Отрисовка элементов---------------
		mainWindow.draw(leftPanel);
		mainWindow.draw(rightPanel);
		mainWindow.draw(instructionText);
		authorField.Draw(mainWindow);
		yearField.Draw(mainWindow);
		bookTitleField.Draw(mainWindow);
		addBookButton.Draw(mainWindow);

		// Если статус добавления книги есть, то отрисовываем его
		if (showStatus)
		{
			statusText.setString(statusMessage);
			mainWindow.draw(statusText);
		}

		// Создаем объект текста для вывода количества книг
		sf::Text bookCountText(font, "", 18);
		bookCountText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
		bookCountText.setString(sf::String(L"Всего книг: ") + sf::String(std::to_string(booksArray.size())));
		bookCountText.setPosition({ 440.0f, 30.0f });
		mainWindow.draw(bookCountText);

		// Если массив книг пуст, то создаем текст один, в другом случае - другой
		if (booksArray.empty())
		{
			sf::Text emptyText(font, L"Библиотека пуста", 18);
			emptyText.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
			emptyText.setPosition({ 440.0f, 60.0f });
			mainWindow.draw(emptyText);
		}
		else
		{
			float y = 60.0f;
			// Перечисляем все библиотеки из массива и создаем под каждую из них свой объект текста
			for (int i = 0; i < booksArray.size(); ++i)
			{
				const Book& book = booksArray[i];
				// Формат вывода книги: 1. Название / автор. - год
				sf::String bookInfo = sf::String(std::to_string(i + 1)) + ". " + book.title + " / " + book.author + ". - " + book.year;
				sf::Text bookLine(font, bookInfo, 18);
				bookLine.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
				bookLine.setPosition({ 440.0f,  y });
				mainWindow.draw(bookLine);

				y += 25.0f;
			}
		}

		mainWindow.display();
	}	

	return 0;
}
