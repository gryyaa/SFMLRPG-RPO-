#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cwctype>

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

static sf::String ToLowerString(const sf::String& value)
{
	sf::String normalized = value;
	for (std::size_t i = 0; i < normalized.getSize(); ++i)
	{
		normalized[i] = static_cast<char32_t>(std::towlower(static_cast<wchar_t>(normalized[i])));
	}
	return normalized;
}

static std::vector<Book> FilterVisibleBooks(const std::vector<Book>& books, const sf::String& searchText)
{
	sf::String normalizedSearchText = ToLowerString(searchText);
	std::vector<Book> visibleBooks;
	for (const Book& book : books)
	{
		sf::String normalizedTitle = ToLowerString(book.title);
		sf::String normalizedAuthor = ToLowerString(book.author);
		sf::String normalizedYear = ToLowerString(book.year);
		if (normalizedSearchText.isEmpty() || normalizedTitle.find(normalizedSearchText) != sf::String::InvalidPos || normalizedAuthor.find(normalizedSearchText) != sf::String::InvalidPos || normalizedYear.find(normalizedSearchText) != sf::String::InvalidPos)
		{
			visibleBooks.push_back(book);
		}
	}
	return visibleBooks;
}
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

	sf::RectangleShape buttonShape;
private:
	sf::Text label;	
};

// ---------------Класс анимации---------------
class Animation // У анимации есть: несколько текстур (кадров), текущий кадр, скорость смены кадров, общая длительность (используется для условия проигрывания анимации), таймеры
{
public:
	Animation() {}
	Animation(int currentFrame, float frameDelay, float totalDuration)
	{
		this->currentFrame = currentFrame;
		this->frameDelay = frameDelay;
		this->totalDuration = totalDuration;
	}

	// Функция по загрузке файлов кадров, принимающая массив строк, ссылающихся на пути до кадров
	bool LoadFrames(const std::vector<std::string>& filePaths)
	{
		// Первым делом имеющиеся загруженные кадры в объект очищаются, для того чтобы можно было повторно использовать объект класса Animation. Это выгоднее чем создание полностью нового объекта, потому что не выделяется новая память под объект.
		frames.clear();
		// После полной очистки массива становится неизвестно количество объектов. Для этого заранее резервируем память под количество поступающих объектов в параметр функции.
		frames.reserve(filePaths.size());

		for (const std::string& filePath : filePaths)
		{
			// Создаем объект умного указателя на текстуру
			std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
			// Пытаемся загрузить в объект текстуры файл
			if (!texture->loadFromFile(filePath))
			{
				std::cerr << "Файл текстуры не найден: " << filePath << std::endl;
				return false;
			}
			frames.push_back(texture);
		}

		return !frames.empty();
	}

	bool GetIsActive() const
	{
		return isActive;
	}

	// Функция, выставляющая значения в изначальное положение
	void StartAnimation()
	{
		isActive = true;
		frameClock.restart();
		totalClock.restart();
		currentFrame = 0;
	}	

	// Функция, производящая продвижение по кадрам
	bool UpdateAnimation()
	{
		// Если анимация не активна, то возвращаем false
		if (!isActive) return false;

		// Опиарясь на таймер смены кадра, если он становится больше, чем время задержки (скорости смены кадра), то прибавляем к текущему кадру + 1
		if (frameClock.getElapsedTime().asSeconds() > frameDelay)
		{
			// При простом увеличении кадра рано или поздно появится ситуация, при которой значение текущего кадра станет больше, чем общее кол-во кадров. Необходимо это разрешить. Это можно сделать благодаря нахождению процента от общего количества кадров
			currentFrame = (++currentFrame) % frames.size(); // Благодаря делению с остатком текущий кадр не уйдет в большее значение, чем количество элементов в массиве
			frameClock.restart();
		}

		// определяем когда заканчивается анимация и выставляем значение isActive = false
		// если время анимации подошло к концу, то значит она верно отработала
		if (totalClock.getElapsedTime().asSeconds() > totalDuration)
		{
			isActive = false;
			return true;
		}

		return false;
	}

	// Функция отрисовки анимации в графическое окно
	void Draw(sf::RenderWindow& window)
	{
		if (!isActive) return;

		// Создаем объект спрайта и помещаем в него текстуру из массива кадров, обращаясь по индексу текущего кадра
		sf::Sprite frameSprite(*frames[currentFrame]);
		frameSprite.setScale({ 1.0f, 1.0f });
		frameSprite.setPosition({500.0f, 260.0f});

		window.draw(frameSprite);
	}
private:
	std::vector<std::shared_ptr<sf::Texture>> frames; // массив умных указателей на текстуры
	int currentFrame = 0;
	bool isActive = false;
	float frameDelay = 0.1f; // скорость смены (задержки) кадров
	float totalDuration = 1.4f;
	sf::Clock frameClock; // таймер смены кадра
	sf::Clock totalClock; // таймер учета всего прошедшенго времени
};

int main()
{
	// ---------------Загрузка ресурсов---------------
	sf::Font font("Comic Sans MS.ttf");
	sf::Texture bookTexture("book1.png");
	Animation loadingAnimation;
	// В моем проекте кадры анимации загрузки находятся в папке animations в корне проекта
	loadingAnimation.LoadFrames({
		"animations\\1.png",
		"animations\\2.png",
		"animations\\3.png",
		"animations\\4.png",
		"animations\\5.png",
		"animations\\6.png",
		});

	// ---------------Инициализация основного окна---------------
	const unsigned int WINDOW_WIDTH = 800;
	const unsigned int WINDOW_HEIGHT = 600;
	sf::RenderWindow mainWindow(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), L"Домашняя библиотека", sf::Style::Close);
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
	booksArray.push_back(Book{L"Искусство языка Си", L"Сунь Си", L"2027.04.03"});
	booksArray.push_back(Book{L"Отруби", L"Валентин Касевьев Омарович", L"-2000.-0.1.-5"});

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

			// Только тогда, когда все время, отведенное на анимацию, завершится, мы добавляем книгу в массив для отрисовки на экране
			booksArray.push_back(Book{ bookTitleField.GetValue(), authorField.GetValue(), yearField.GetValue() });

			bookTitleField.ClearInputValue();
			yearField.ClearInputValue();
			authorField.ClearInputValue();

			loadingAnimation.StartAnimation();
			sf::String message(L"Добавление книги...");
			SetStatus(message);
		};				

	auto OpenBook = [&]()
	{
		sf::RenderWindow bookWindow(sf::VideoMode({ 400, 200 }), L"Карточка книги", sf::Style::Close);
		while (mainWindow.isOpen())
		{
			while (const std::optional event = bookWindow.pollEvent())
			{
				if (event->is<sf::Event::Closed>())
				{
					bookWindow.close();
				}
			}
		}		
		bookWindow.clear(sf::Color(SUNFLOWER_GOLD_COLOR));
		bookWindow.display();
	};

	Button addBookButton(font, L"Добавить книгу", { 90.0f, 500.0f }, { 200.0f, 26.0f });
	Button openBookButton = { font, L"Открыть", {680.0f, 60.0f}, {90, 30} };

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
			// ИЗМЕНЕНО: пользователь может взаимодействовать с программой только если анимация загрузки книги в библиотеку не активна!
			else if (!loadingAnimation.GetIsActive())
			{
				// дополнительная обработка события нахождения мыши в пределах окна и нажатия. getIF - вспомогательная функция, определяющая наличие события с дополнительным уссловием
				if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
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
						else if (openBookButton.Contains(mousePoint))
						{
							OpenBook();
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
		}

		if (loadingAnimation.UpdateAnimation())
		{
			sf::String message(L"Книга успешно добавлена!");
			SetStatus(message);
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
			// Перечисляем все библиотеки из массива и создаем под каждую из них свой объект текста
			float bookFieldY = 60.0f;
			for (int i = 0; i < booksArray.size(); ++i)
			{
				const Book& book = booksArray[i];
				// Формат вывода книги: 1. Название / автор. - год
				//sf::String bookInfo = sf::String(std::to_string(i + 1)) + ". " + book.title + " / " + book.author + ". \n- " + book.year;👿
				sf::String bookInfo = sf::String(std::to_string(i + 1)) + ". " + book.title;

				// Отрисовка спрайта в программе.
				// По аналогии с отрисовкой текста в графическом приложении, где есть два вида классов Text и String, у отрисовки пользовательской (своей) графики есть класс Texture, отвечающий за содержимое изображения, и класс Sprite, отвечающий за рендер или отрисовку в графическом окне.
				sf::Sprite bookSprite(bookTexture);
				bookSprite.setScale({ 0.01f, 0.01f });
				bookSprite.setPosition({ 430.0f, bookFieldY });
				mainWindow.draw(bookSprite);

				sf::Text bookLine(font, bookInfo, 18);
				bookLine.setFillColor(sf::Color(CHARCOAL_BROWN_COLOR));
				bookLine.setPosition({ 480.0f,  bookFieldY });

				openBookButton.buttonShape.setPosition({ 680.0f, bookFieldY });
				openBookButton.Draw(mainWindow);
				mainWindow.draw(bookLine);

				bookFieldY += 50.0f;
			}
		}
		loadingAnimation.Draw(mainWindow);
		mainWindow.display();
	}	

	return 0;
}
