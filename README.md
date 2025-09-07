# 项目文档大纲

## 1. 项目概述

### 1.1 项目背景

​	随着触摸屏设备的普及，手写输入作为一种自然的输入方式，逐渐成为人们日常操作中不可忽视的输入方式之一。尤其是在平板电脑、智能手机及各种触摸设备上，手写输入能够提供更加便捷、直观的用户体验。在传统的输入法中，用户需要通过键盘或拼音输入进行文字输入，但对于中文、繁体字或符号的输入，拼音输入法或键盘输入往往显得不够灵活或高效。尤其对于不熟悉拼音或需要快速输入的人来说，手写输入是一种更为自然、直观且高效的方式。

​	因此，开发一款高效、准确的智能手写输入法，能够帮助用户更方便地输入文字，提高工作和学习效率。本项目通过集成手写识别和智能联想功能，弥补了传统输入法在某些场景下的不足，并具有广泛的应用价值。

### 1.2 项目目标

​	本项目的设计目标是打造一款智能手写输入法，提供高效且精准的手写识别，支持用户在不同应用场景下的手写输入需求。其核心功能和目标包括：

- **精准的单字和长句手写识别**：通过手写识别引擎，精准地识别用户输入的每一个字，并能识别连续输入的多个字。
- **智能联想与预测**：根据上下文和用户的输入习惯，实时提供可能的词组或短语，提高输入效率。
- **中英文混合输入**：支持中英文混合输入，能够识别并输入英文字母、数字以及符号。
- **高度个性化的设置功能**：提供笔迹颜色、粗细、候选字号等多种设置选项，适应不同用户的输入习惯。
- **流畅的用户体验**：通过优化手写识别速度和候选字更新机制，确保输入的流畅性与高效性。

预期效果是提供一种自然、流畅、高效的手写输入体验，满足用户在移动设备、触摸设备或平板电脑上的手写输入需求，同时兼顾不同输入场景的便捷性和准确性。

### 1.3 技术栈

本项目使用了多种技术来实现手写输入识别与智能联想功能，包括：

- **Qt (C++)**：Qt作为跨平台开发框架，提供了强大的图形界面设计和跨平台支持。在本项目中，Qt被用于开发图形用户界面（GUI）以及实现手写区域的绘制、笔迹识别等功能。
- **Windows Tablet PC SDK & Microsoft Ink**：这些是微软为手写输入提供的工具和API，通过它们可以实现高效的笔迹识别。本项目通过集成Microsoft Ink API来捕捉用户的手写输入并转化为数字化文本。
- **SQLite**：SQLite是一个轻量级的数据库系统，本项目使用它来存储和管理用户输入的数据、词组库及其优先级等信息。通过SQLite的高效查询和数据管理功能，实现了输入法的联想词库和历史记录功能。

这些技术的结合，保证了输入法的高效性、准确性和用户友好性，使得手写输入成为一种可行且高效的输入方式。选择这些技术是因为它们具有高性能、广泛的支持和与Windows平台的深度集成，可以提供最佳的用户体验。

## 2. 系统设计

### 2.1 系统架构

- 高层次的架构图，展示各组件之间的关系。
- 主要模块介绍：`HandwritingArea`、`HandwritingRecognizer`、`TextPredictionEngine`等。

### 2.2 主要功能模块

#### 2.2.1 手写输入区域 (`HandwritingArea`)

`HandwritingArea` 模块负责用户的手写输入区域，它实现了手写绘制、笔迹撤销、重写、笔迹颜色和宽度设置等功能，并通过与 `HandwritingRecognizer` 交互实现手写识别。

##### 主要功能与实现

1. **手写绘制**：

   用户在 `HandwritingArea` 上点击鼠标或使用触控设备进行手写输入时，程序会捕获鼠标事件并通过 `mousePressEvent`、`mouseMoveEvent`、`mouseReleaseEvent` 等事件处理函数将用户的手写轨迹绘制出来。每次绘制的轨迹被存储为 `QPainterPath` 对象并保存在 `strokes` 列表中。

2. **手写识别**：

   每当用户完成一次笔迹输入（即松开鼠标左键），`mouseReleaseEvent` 会触发并调用手写识别器 `HandwritingRecognizer` 进行识别。识别结果通过 `needToReco` 信号传递给外部处理模块（如主窗口）进行后续操作，如候选词更新等。

3. **撤销功能**：

   用户可以点击撤销按钮来撤回最后一次的笔迹输入。撤销操作会从 `strokes` 列表中移除最后一笔，并通知识别器更新其数据。若撤销后没有剩余笔迹，则会调用重写函数清空所有数据。

4. **重写功能**：

   用户可以通过点击重写按钮清空所有笔迹并重新开始手写输入。这会调用 `rewrite` 函数，清空 `strokes` 列表和手写识别器中的数据，并触发界面的更新。

5. **笔迹颜色和宽度设置**：

   `HandwritingArea` 提供了设置笔迹颜色和宽度的功能。用户可以通过界面上的设置控件来调整笔迹的颜色（`m_penColor`）和宽度（`m_penWidth`），并通过 `setPenColor` 和 `setPenWidth` 函数应用新的设置。

6. **事件过滤与控件焦点管理**：

   为确保在控件被禁用时鼠标事件不会被传递，`eventFilter` 函数会过滤掉鼠标按键事件。在禁用状态下，控件不会响应用户输入。

7. **UI更新**：

   每次用户进行绘制、撤销或修改设置时，都会调用 `update()` 函数进行界面的重新绘制，确保笔迹和界面状态保持同步。

##### 关键函数说明

- `rewrite()`：清空所有笔迹并重绘手写区域。
- `undo()`：撤销最后一笔手写输入。
- `setPenWidth(int width)`：设置笔迹宽度。
- `setPenColor(QColor color)`：设置笔迹颜色。
- `paintEvent(QPaintEvent* event)`：自定义绘制函数，用于将手写笔迹显示在界面上。
- `mousePressEvent(QMouseEvent* event)`：鼠标按下时开始绘制路径。
- `mouseMoveEvent(QMouseEvent* event)`：鼠标移动时继续绘制路径。
- `mouseReleaseEvent(QMouseEvent* event)`：鼠标松开时结束路径并传递笔迹数据给识别器。

##### 与其他模块的关系

- `HandwritingArea` 通过信号 `needToReco` 将笔迹数据传递给 `HandwritingRecognizer` 进行识别。
- `HandwritingArea` 控制手写区域的显示和更新，并与 `KMainWindow` 进行交互，将识别结果或候选词汇反馈给主界面。
- 设置相关功能通过 `KMainWindow` 调用，允许用户调整笔迹的颜色和宽度。

#### 2.2.2 手写识别 (`HandwritingRecognizer`)

`HandwritingRecognizer` 模块负责与 Microsoft Ink API 交互，进行手写输入的识别处理。该模块通过初始化墨迹收集器、绑定识别上下文以及执行手写识别，来识别用户的手写内容并提供候选词汇。

##### 主要功能与实现

1. **初始化墨迹收集器与识别上下文**：

   `inputInit(HWND hwnd)`：该函数初始化墨迹收集器，并将其与指定的窗口句柄绑定。通过创建 `IInkRecognizerContext` 和 `IInkCollector` 实例，设置识别器的环境，并确保墨迹收集功能被启用。

2. **语言设置**：

   `SetRecognizerLanguage(LANGID langId)`：该函数设置手写识别器的语言。通过查询可用的识别器，选择合适的语言识别模型，例如中文简体手写识别器，并将其应用到识别上下文。

3. **单字符识别模式**：

   `setSingleCharacterRecognitionMode()`：此函数将识别模式设置为单字符模式。在该模式下，识别器仅处理一个字符的输入。通过设置 `InkRecoGuide` 对象来定义字符的书写区域。

4. **多字符识别模式**：

   `setMultCharacterRecognitionMode()`：将识别模式切换为多字符模式。在该模式下，识别器可以处理多个字符的连续输入。`InkRecoGuide` 也在此模式下进行相应的配置。

5. **墨迹管理与清除**：

   `clearInk()`：清除所有已收集的墨迹数据，重置识别器的状态。

   `removeLastStroke()`：撤销最近的一笔输入，删除最后一条笔迹。

6. **手写识别**：

   `RegDataEx()`：该函数通过 `IInkRecognizerContext` 执行手写识别，获取识别结果及候选词汇。识别结果通过 `IInkRecognitionAlternates` 返回，包含多个候选的识别结果。每次用户完成手写并松开鼠标，识别器会被调用执行识别，并返回可能的词汇供系统进行选择。

##### 关键函数说明

- `clearInk()`：清除所有已存储的墨迹。
- `removeLastStroke()`：删除最后一笔手写输入。
- `inputInit(HWND hwnd)`：初始化墨迹收集器并绑定窗口句柄。
- `SetRecognizerLanguage(LANGID langId)`：设置识别器语言，确保使用正确的语言模型。
- `setSingleCharacterRecognitionMode()`：设置单字符识别模式。
- `setMultCharacterRecognitionMode()`：设置多字符识别模式。
- `RegDataEx()`：执行手写识别并返回候选结果。

##### 与其他模块的关系

- `HandwritingRecognizer` 与 `HandwritingArea` 配合，接收来自手写输入区域的墨迹数据，并进行手写识别。
- 识别结果通过 `RegDataEx()` 返回，`HandwritingArea` 将根据候选结果更新输入区域的候选词列表。
- `HandwritingRecognizer` 提供的 `setPenWidth`、`setPenColor` 等方法允许 `HandwritingArea` 根据用户输入调整识别区域的外观。

#### 2.2.3 文本预测引擎 (`TextPredictionEngine`)

`TextPredictionEngine` 模块负责根据用户的输入，提供候选的词汇预测。它通过使用 SQLite 数据库存储和管理词汇，并根据输入的前缀返回优先级较高的候选词汇。

##### 主要功能与实现

1. **初始化数据库连接**：

   在构造函数中，`TextPredictionEngine` 会检查指定的数据库文件是否存在。如果数据库文件不存在，它会创建一个新的 SQLite 数据库，并根据提供的文本文件将词汇导入到数据库中。

2. **创建与初始化数据库表**：

   `initializeDatabase()`：此方法会创建一个名为 `WordDictionary` 的表格，用于存储词汇和它们的优先级。

   `ensureDatabaseExists()`：此方法确保数据库已存在并且表格已初始化。

3. **从文本文件导入词汇**：

   `importWordsFromFile(const QString& filePath)`：通过读取指定的文本文件，将文件中的每一行（即每个词汇）导入到数据库中的 `WordDictionary` 表。使用 `INSERT OR IGNORE` 确保不会插入重复的词汇。

4. **更新词汇的优先级**：

   `updatePriority(const QString& character)`：该方法通过更新数据库中的优先级字段，增加所有以指定字符为前缀的词汇的优先级。它的作用是根据用户的输入或行为，增加常用词汇的优先级。

5. **词汇预测**：

   `predict(const QString& input)`：此方法根据用户输入的前缀查询数据库，返回最多 4 个候选词汇。这些候选词汇会根据它们的优先级进行排序，优先级较高的词汇排在前面。

##### 关键函数说明

- **构造函数**：

  `TextPredictionEngine(const QString& dbPath, const QString& txtFilePath)`：构造函数初始化数据库连接，并在数据库不存在时创建数据库和导入词汇。

- **数据库初始化**：

  `initializeDatabase()`：创建 `WordDictionary` 表。

  `ensureDatabaseExists()`：确保数据库和表存在。

- **词汇导入**：

  `importWordsFromFile(const QString& filePath)`：从指定的文本文件导入词汇到数据库中。

- **优先级更新**：

  `updatePriority(const QString& character)`：更新以某个字符为前缀的词汇优先级。

- **词汇预测**：

  `predict(const QString& input)`：根据输入的前缀查询数据库，返回候选的词汇。

##### 数据库结构

`WordDictionary` 表格的结构如下：

- `id`: 主键，自动递增的唯一标识。
- `word`: 存储词汇。
- `priority`: 存储该词汇的优先级，默认为 0。

##### 与其他模块的关系

- `TextPredictionEngine` 与 `HandwritingRecognizer` 配合，通过手写输入提供前缀，然后根据这些前缀预测候选词汇。
- `TextPredictionEngine` 的 `predict` 方法提供了基于用户输入的词汇建议，可以在手写输入法中实时更新候选词汇列表。
- `updatePriority` 方法可用于调整数据库中词汇的优先级，例如，如果某个词汇被频繁输入，可以增加其优先级。

##### 示例用法

- 假设用户输入“手”，`predict` 方法会查询数据库，返回以“手”开头的词汇（如“手写”、“手指”）。
- 随着用户输入更多字符，预测结果会根据输入内容动态更新，推荐更相关的词汇。

#### 2.2.4 设置对话框 (`SettingsDialog`)

`SettingsDialog` 类提供了一个界面，允许用户调整笔迹设置（宽度、颜色）和字体大小。它通过不同的信号与槽机制与主界面进行交互，实现动态设置。

##### 主要功能与实现

1. **构造函数**：

   - 初始化 UI，设置默认值，填充下拉框数据。
   - 初始化信号与槽的连接，使得用户选择不同设置时能够响应。
   - 设置颜色按钮，用户点击不同的按钮会更改笔迹颜色。
   - 初始化默认选项：笔迹宽度为 "中"，字体大小为 "小"。

2. **设置笔迹宽度**：

   `onPenWidthChanged(int index)`：响应笔迹宽度的变化，更新 `m_penWidth` 并标记设置已更改。

   `m_penWidth` 是从下拉框 `comboBoxPenWidth` 中获取的用户选择值。

3. **设置笔迹颜色**：

   `onPenColorChanged()`：根据用户点击的按钮更改笔迹颜色。每个颜色按钮对应一个 `QPushButton`，点击后会更改 `m_penColor`。

4. **设置字体大小**：

   `onFontSizeChanged(int index)`：响应字体大小的变化，更新 `m_fontSize`，并标记设置已更改。

5. **应用设置**：

   `applyChange()`：如果用户对设置做出更改，点击 "应用" 按钮时，会发出信号通知主界面，传递当前的设置（如笔迹宽度、颜色和字体大小）。

6. **恢复默认设置**：

   `btnSetSettingsDefault` 按钮将设置恢复为默认值：笔迹宽度为 "中"、字体大小为 "小"、颜色为黑色，并更新 UI 以反映这些变化。

7. **关闭事件**：

   `closeEvent(QCloseEvent* event)`：当关闭对话框时，重置 `pen_hasChanged` 和 `font_hasChanged`，确保下次打开时状态正常。

##### 关键函数说明

- **构造函数**：
  - 设置下拉框和按钮，并连接相应的信号与槽。
  - `ui.setupUi(this)` 通过 UI 文件设置界面。
- **设置笔迹宽度**：
  - `onPenWidthChanged(int index)`：从 `comboBoxPenWidth` 中获取用户选择的笔迹宽度并更新。
- **设置笔迹颜色**：
  - `onPenColorChanged()`：根据按钮的点击事件修改 `m_penColor`。
- **设置字体大小**：
  - `onFontSizeChanged(int index)`：获取并更新字体大小。
- **应用设置**：
  - `applyChange()`：如果有任何设置变化，发出信号通知主界面。
- **关闭事件**：
  - `closeEvent()`：关闭对话框时重置相关标记，避免无效的设置保存。

##### UI 控件说明

- **`comboBoxPenWidth`**：一个下拉框，允许用户选择笔迹宽度（粗、中、细）。
- **`comboBoxFontSize`**：一个下拉框，允许用户选择字体大小（大、中、小）。
- **`btnApplyNewSettings`**：一个按钮，点击后应用新的设置。
- **`btnSetSettingsDefault`**：一个按钮，点击后恢复默认设置（笔迹宽度、中等、字体大小、小、颜色为黑色）。
- **颜色按钮**：多个按钮 (`btnSetColor_black`, `btnSetColor_red` 等)，允许用户选择不同的笔迹颜色。

##### 信号与槽

- **信号**：
  - `penSettingsChanged(int penWidth, QColor penColor)`：当笔迹设置（宽度、颜色）发生变化时发出信号。
  - `buttonFontSizeChanged(int fontSize)`：当字体大小发生变化时发出信号。
- **槽函数**：
  - `onPenWidthChanged(int index)`：响应笔迹宽度的变化。
  - `onPenColorChanged()`：响应颜色按钮的点击事件。
  - `onFontSizeChanged(int index)`：响应字体大小的变化。
  - `applyChange()`：应用用户所做的所有设置更改。

#### 2.2.5 整体界面 (`KMainWindow`)

##### 主要功能与实现

`KMainWindow` 是手写输入法应用的主窗口类，负责应用界面的呈现与各项交互逻辑的管理。作为整个应用的核心组件，它协调了手写输入区域、候选字词的展示、输入模式的切换、以及设置管理等多个模块。以下是 `KMainWindow` 的核心功能：

- **手写输入区域**：

  通过 `HandwritingArea` 类实现，用户可以在该区域进行手写输入。`HandwritingArea` 支持单个字符模式和多个字符模式的识别，同时提供撤销和重写功能。

- **候选字词更新**：

  根据手写输入的识别结果，`KMainWindow` 会动态更新界面上各个候选字词按钮的文字。用户点击这些按钮可以将候选字词输入到当前焦点窗口。

- **输入模式切换**：

  支持多种输入模式，包括单字符模式和多个字符模式的切换。此外，用户还可以在中文、英文、数字和标点模式之间进行切换。

- **设置管理**：

  通过 `SettingsDialog` 类提供设置界面，用户可以修改输入法的相关设置，例如笔迹的宽度、颜色、按钮字体大小等。

- **文本预测引擎**：

  使用 `TextPredictionEngine` 提供的词汇预测功能，在用户输入时自动提供候选词汇，提升输入效率。该引擎根据用户的输入上下文和历史行为不断调整候选词汇的优先级。

##### 关键函数说明

- `setupConnections()`：连接各个按钮与相应的槽函数，实现界面控件之间的交互。例如，点击候选字词按钮时，会将该字词输入到目标窗口。
- `storeInitialButtonTexts()`：初始化时保存所有按钮的默认文本，为后续的文本更新提供参考。
- `switchToSingleMode()` / `switchToMultMode()`：切换输入模式，支持单字符输入和多字符输入。
- `switchToChineseMode()` / `switchToEnglishMode()` / `switchToNumberMode()` / `switchToPunctuationMode()`：切换不同的输入模式，分别为中文模式、英文模式、数字模式和标点符号模式。
- `getRecognitonResult()`：处理手写识别结果，更新候选字词按钮的文本。
- `updateShortCandidateButtons()`：更新短候选字词按钮的文本，并根据预测引擎提供的候选字词更新预测按钮的文本。
- `resetButtonTextsToDefault()`：将按钮文本恢复到初始状态。
- `clickCandidateButtonsInput()`：处理候选字词按钮的点击事件，将选中的候选字词输入到目标窗口。
- `SendKeys()`：模拟键盘输入，将选中的候选字词发送到当前获得焦点的窗口。

##### 与其他模块的关系

- **与 `HandwritingArea` 的关系**：`KMainWindow` 控制手写输入区域的显示和隐藏。手写区域通过 `HandwritingArea` 进行绘制与识别，而识别结果会通过信号 `needToReco` 传递给 `KMainWindow`，更新候选字词按钮。
- **与 `TextPredictionEngine` 的关系**：`KMainWindow` 使用 `TextPredictionEngine` 提供的预测功能来实现输入的联想词汇，增强输入法的智能性。手写输入的候选字词会传递给预测引擎，生成预测结果。
- **与 `SettingsDialog` 的关系**：通过 `SettingsDialog`，用户可以修改输入法的设置（如笔迹宽度、颜色等），并且 `KMainWindow` 会实时应用这些设置，调整手写输入区域的显示效果。

##### UI控件说明

- **`keyEntryButtonWidget`**：包含字母、数字、标点符号等输入按钮。当切换到英文、数字或标点模式时，会显示这些按钮。
- **`handwritingArea`**：手写输入区域，用户在该区域进行手写输入。支持单字符模式和多字符模式。
- **`frameCandidates`**：用于展示候选字词的按钮框架。每个按钮代表一个候选字词，用户可以点击选择。
- **`stackedWidget`**：通过该控件实现界面切换，支持不同的输入模式（单字符模式、多个字符模式）。
- **`btnUndo` / `btnRewrite`**：撤销和重写按钮，允许用户修改或清除手写输入。
- **`btnPossibleShow`**：显示候选字词中的最可能的候选项。
- **`btnPredictCandidate1` ~ `btnPredictCandidate4`**：用于展示文本预测的候选字词按钮。

##### 信号与槽

- `KMainWindow` 的信号：
  - `needToUpdateCandidateButtons`：当手写识别结果准备好时，发出此信号更新候选字词按钮。
- `KMainWindow` 的槽函数：
  - `updateShortCandidateButtons`：响应手写识别结果或预测结果，更新短候选字词按钮的文本。
  - `getRecognitonResult`：接收手写输入的识别结果，触发候选字词更新的槽函数。
  - `clickCandidateButtonsInput`：处理用户点击候选字词按钮的事件，模拟按键输入到当前焦点窗口。
  - `resetButtonTextsToDefault`：重置所有按钮的文本为初始状态。
- `HandwritingArea` 的信号：
  - `needToReco`：当手写区域完成输入时，发出信号请求识别。
  - `inkClear`：清除手写内容，恢复默认按钮文本。
- `SettingsDialog` 的信号：
  - `penSettingsChanged`：当用户调整笔迹设置时，发出此信号更新手写区域的笔迹宽度和颜色。
  - `buttonFontSizeChanged`：当用户修改按钮字体大小时，发出此信号调整所有按钮的字体大小。

## 3. 核心功能代码解释

### 3.1 笔迹绘制功能

 **`paintEvent`：绘制手写路径**

```
void HandwritingArea::paintEvent(QPaintEvent* event)
{
    if (!this->isEnabled()) return;  // 确保控件是启用状态

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 开启抗锯齿，画笔更加平滑
    painter.setPen(QPen(m_penColor, m_penWidth));  // 设置笔的颜色与粗细

    // 绘制所有存储的笔迹
    for (const auto& path : strokes) {
        painter.drawPath(path);
    }

    // 如果当前正在绘制，就绘制当前路径（手指或鼠标移动过程中）
    if (isDrawing) {
        painter.drawPath(currentPath);
    }
}
```

核心功能：

- **绘制笔迹**：`paintEvent` 方法负责在手写区域绘制路径。它会先检查控件是否处于启用状态，然后使用 `QPainter` 进行绘制。
- **抗锯齿**：开启抗锯齿使得画笔的边缘更加平滑，提升用户体验。
- **路径绘制**：使用 `painter.drawPath` 方法来绘制存储的笔迹 `strokes` 列表中的所有路径，以及当前用户正在绘制的路径（`currentPath`）。

2. **`mousePressEvent`：鼠标按下事件**

```
void HandwritingArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isDrawing = true;  // 设置绘制状态为 true
        currentPath = QPainterPath(event->pos());  // 创建一个新的路径，从当前点击位置开始
    }
}
```

核心功能：

- **开始绘制**：当用户按下鼠标左键时，`isDrawing` 被设置为 `true`，表明绘制已开始。
- **创建路径**：创建一个新的 `QPainterPath` 对象，并将当前鼠标点击的位置作为路径的起点，准备开始绘制路径。

3. **`mouseMoveEvent`：鼠标移动事件**

```
void HandwritingArea::mouseMoveEvent(QMouseEvent* event)
{
    if (isDrawing) {
        currentPath.lineTo(event->pos());  // 向当前路径添加新的点
        update();  // 触发绘制更新
    }
}
```

核心功能：

- **实时更新路径**：在鼠标移动过程中，如果 `isDrawing` 为 `true`，即用户正在绘制路径，则使用 `lineTo` 方法将鼠标的当前位置添加到当前路径 `currentPath` 中。
- **更新绘制**：调用 `update()` 方法触发 `paintEvent`，重新绘制手写区域并显示新的路径。

4. **`mouseReleaseEvent`：鼠标释放事件**

```
void HandwritingArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isDrawing = false;  // 停止绘制
        strokes.append(currentPath);  // 将当前路径加入到笔迹列表
        currentPath = QPainterPath();  // 清空当前路径

        // 将笔迹传递给识别器进行处理
        update();  // 触发重新绘制

        IInkRecognitionAlternates* recognitionResult = recognizer->RegDataEx();  // 识别器接收笔迹列表
        emit needToReco(recognitionResult);  // 发出绘制完成/状态变更信号
    }
}
```

核心功能：

- **结束绘制**：当用户松开鼠标左键时，`isDrawing` 被设置为 `false`，停止绘制当前路径。
- **保存路径**：当前路径 `currentPath` 被添加到 `strokes` 列表中，表示这条手写路径已完成。
- **触发识别**：调用 `recognizer->RegDataEx()` 方法将当前的笔迹传递给识别器进行处理，返回识别结果。
- **更新绘制**：调用 `update()` 重新绘制手写区域，显示完成的笔迹。

### 3.2 手写识别功能

- 手写识别功能通过调用 Microsoft Ink API 实现，将用户的手写笔迹转换为文字。以下是代码中关键部分的讲解，包括如何初始化手写识别器、处理笔迹以及获取识别结果。

  1. **初始化手写识别器和墨迹收集器**

  ```
  void HandwritingRecognizer::inputInit(HWND hwnd)
  {
      CoInitialize(NULL);
      HRESULT hr;
  
      // 创建识别器上下文
      hr = CoCreateInstance(CLSID_InkRecognizerContext, NULL, CLSCTX_INPROC_SERVER, IID_IInkRecognizerContext, (void**)&g_pIInkRecoContext);
      if (FAILED(hr)) {
          qDebug() << "Failed to create InkRecognizerContext";
          return;
      }
  
      // 创建墨迹收集器
      hr = CoCreateInstance(CLSID_InkCollector, NULL, CLSCTX_INPROC_SERVER, IID_IInkCollector, (void**)&g_pIInkCollector);
      if (FAILED(hr)) {
          qDebug() << "Failed to create InkCollector";
          return;
      }
  
      // 获取墨迹对象
      hr = g_pIInkCollector->get_Ink(&g_pIInkDisp);
      if (FAILED(hr)) {
          qDebug() << "Failed to get Ink";
          return;
      }
  
      // 绑定窗口句柄
      hr = g_pIInkCollector->put_hWnd((long)hwnd);
      if (FAILED(hr)) {
          qDebug() << "Failed to bind HWND";
          return;
      }
  
      // 开启墨迹收集
      hr = g_pIInkCollector->put_Enabled(VARIANT_TRUE);
      if (FAILED(hr)) {
          qDebug() << "Failed to enable InkCollector";
      }
  }
  ```

  **功能介绍**：

  - **目的**：初始化手写识别器和墨迹收集器，准备接收用户输入的笔迹。
  - **创建识别上下文**：通过 `CoCreateInstance` 创建 `IInkRecognizerContext` 对象，负责处理识别上下文。
  - **墨迹收集器**：创建 `IInkCollector` 实例，并绑定到指定的窗口句柄。这样，用户的手写输入就可以通过该对象进行收集和处理。
  - **启用墨迹收集**：调用 `put_Enabled(VARIANT_TRUE)` 开启墨迹收集功能，允许用户在界面中进行手写输入。

  #### 2. **设置识别语言**

  ```
  HRESULT HandwritingRecognizer::SetRecognizerLanguage(LANGID langId)
  {
      CComPtr<IInkRecognizers> spIInkRecognizers;
      HRESULT hr = spIInkRecognizers.CoCreateInstance(CLSID_InkRecognizers);
      if (FAILED(hr)) {
          qDebug() << "Failed to create InkRecognizers instance.";
          return hr;
      }
  
      // 获取识别器数目
      LONG count = 0;
      hr = spIInkRecognizers->get_Count(&count);
      if (FAILED(hr)) {
          qDebug() << "Failed to get InkRecognizers count.";
          return hr;
      }
  
      if (count > 0) {
          CComPtr<IInkRecognizer> spIInkRecognizer;
          for (LONG i = 0; i < count; i++) {
              hr = spIInkRecognizers->Item(i, &spIInkRecognizer);
              if (FAILED(hr)) {
                  continue;
              }
  
              CComVariant vLanguages;
              hr = spIInkRecognizer->get_Languages(&vLanguages);
              if (SUCCEEDED(hr) && VT_ARRAY == (VT_ARRAY & vLanguages.vt) && vLanguages.parray && vLanguages.parray->rgsabound[0].cElements > 0) {
                  CComBSTR bstrName;
                  hr = spIInkRecognizer->get_Name(&bstrName);
                  if (SUCCEEDED(hr) && wcscmp(bstrName, L"Microsoft 中文(简体)手写识别器") == 0) {
                      g_pIInkRecoContext->Release();
                      hr = spIInkRecognizer->CreateRecognizerContext(&g_pIInkRecoContext);
                      if (FAILED(hr)) {
                          qDebug() << "Failed to create recognizer context for the chosen recognizer.";
                          return hr;
                      }
                      break;
                  }
              }
  
              spIInkRecognizer.Release();
          }
      }
      return hr;
  }
  ```

  **功能介绍**：

  - **目的**：设置手写识别语言，确保识别器能够识别不同语言的手写输入。
  - **获取识别器列表**：通过 `IInkRecognizers` 获取系统中可用的所有识别器。
  - **选择特定语言的识别器**：在识别器列表中查找适合的语言识别器（如简体中文手写识别器），并将其绑定到 `g_pIInkRecoContext` 上下文中。

  #### 3. **手写识别与获取候选词**

  ```
  IInkRecognitionAlternates* HandwritingRecognizer::RegDataEx()
  {
      IInkStrokes* pIInkStrokes = NULL;
      HRESULT hr = g_pIInkDisp->get_Strokes(&pIInkStrokes);
      if (SUCCEEDED(hr)) { // 检查是否成功获取墨迹笔画对象
          hr = g_pIInkRecoContext->putref_Strokes(pIInkStrokes);
          if (SUCCEEDED(hr)) { // 确保成功将笔画数据绑定到识别上下文
              IInkRecognitionResult* pIInkRecoResult = NULL;
              InkRecognitionStatus RecognitionStatus = IRS_NoError;
  
              // 尝试进行手写识别
              hr = g_pIInkRecoContext->Recognize(&RecognitionStatus, &pIInkRecoResult);
              if (SUCCEEDED(hr) && pIInkRecoResult) {
                  // 获取识别结果的多个候选字符串
                  IInkRecognitionAlternates* spIInkRecoAlternates;
                  hr = pIInkRecoResult->AlternatesFromSelection(0, -1, 10, &spIInkRecoAlternates);
                  if (SUCCEEDED(hr)) {
                      return spIInkRecoAlternates;
                  }
              }
          }
  
          // 释放墨迹笔画对象资源
          pIInkStrokes->Release();
          return NULL;
      }
  }
  ```

  **功能介绍**：

  - **目的**：处理手写识别并获取候选的识别结果。
  - **绑定笔迹数据**：首先，获取 `IInkDisp` 中的笔迹数据，并将其绑定到 `IInkRecognizerContext` 上下文中。
  - **识别笔迹**：通过调用 `g_pIInkRecoContext->Recognize` 方法进行识别，并获取识别结果。
  - **候选结果**：使用 `AlternatesFromSelection` 方法获取多个候选结果，最多返回 10 个候选字符串，以便用户选择。

### 3.3 词组预测功能

**1. 词库初始化与数据库连接**

在 `TextPredictionEngine` 的构造函数中，首先检查数据库文件是否存在。如果数据库文件不存在，则会创建一个新的 SQLite 数据库，并通过调用 `importWordsFromFile` 方法从提供的文本文件中导入词汇。

```
TextPredictionEngine::TextPredictionEngine(const QString& dbPath, const QString& txtFilePath)
    : m_dbPath(dbPath), m_txtFilePath(txtFilePath) {
    // 确保数据库存在，如果不存在则初始化并导入数据
    if (!QFile::exists(m_dbPath)) {
        // 打开数据库连接
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName(m_dbPath);
        if (!m_database.open()) {
            qCritical() << "Failed to open database:" << m_database.lastError().text();
            return;
        }
        ensureDatabaseExists(); // 确保数据库表结构存在
        if (!m_txtFilePath.isEmpty()) {
            importWordsFromFile(m_txtFilePath); // 从文本文件导入词汇
        }
    }
    else {
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName(m_dbPath);
        if (!m_database.open()) {
            qCritical() << "Failed to open database:" << m_database.lastError().text();
            return;
        }
        return;  // 如果数据库已存在，则直接打开数据库
    }
}
```

**解释：**

- `TextPredictionEngine` 类的构造函数首先检查数据库文件是否存在。如果文件不存在，创建数据库连接，调用 `ensureDatabaseExists()` 来确保数据库表格的结构，并且如果提供了文本文件路径，则调用 `importWordsFromFile()` 方法导入词汇。
- 如果数据库已存在，则直接打开数据库，不再进行初始化。

**2.从文本文件导入词汇到数据库**

`importWordsFromFile` 方法从指定的文本文件中逐行读取词汇，并将每个词汇插入到数据库中的 `WordDictionary` 表。为了提高性能，所有插入操作在一个事务中进行。

```
void TextPredictionEngine::importWordsFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Failed to open file:" << filePath << " Error:" << file.errorString();
        return;
    }

    // 设置正确的编码格式，避免中文乱码
    QTextStream in(&file);
    in.setCodec("UTF-8");

    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO WordDictionary (word) VALUES (:word)");

    // 开始一个事务，以提高性能
    m_database.transaction();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            query.bindValue(":word", line);  // 绑定每一行的词汇
            if (!query.exec()) {
                qCritical() << "Failed to insert word:" << query.lastError().text();
            }
        }
    }

    // 提交事务
    m_database.commit();

    file.close();
}
```

**解释：**

- 该方法首先打开指定路径的文件，如果文件打开失败，会输出错误信息。
- 然后通过 `QTextStream` 逐行读取文件内容，并确保使用 UTF-8 编码来避免中文乱码。
- 使用 `QSqlQuery` 准备 SQL 插入语句，通过事务来批量插入词汇，提高性能。
- 最后，关闭文件并提交事务，确保数据被正确写入数据库。

**3.根据输入前缀预测词汇**

`predict` 方法根据用户输入的前缀查询数据库，返回最多 4 个候选词汇。这些候选词汇根据优先级排序，优先级高的词汇排在前面。

```
QVector<QString> TextPredictionEngine::predict(const QString& input) const {
    QVector<QString> predictions;

    // 确保数据库已打开
    if (!m_database.isOpen()) {
        qCritical() << "Database is not open!";
        return predictions;
    }

    // 创建查询对象
    QSqlQuery query;

    // 准备查询
    QString queryStr = R"(
        SELECT word
        FROM WordDictionary
        WHERE word LIKE :prefix
        AND LENGTH(word) <= 3
        ORDER BY priority DESC, word ASC
        LIMIT 4
    )";
    if (!query.prepare(queryStr)) {
        qCritical() << "Failed to prepare query:" << query.lastError().text();
        return predictions;
    }

    // 绑定参数
    query.bindValue(":prefix", input + "%");  // 绑定输入的前缀参数

    // 执行查询
    if (!query.exec()) {
        qCritical() << "Failed to execute query: " << query.lastError().text();
        return predictions;
    }

    // 获取查询结果
    while (query.next()) {
        predictions.append(query.value(0).toString());  // 将查询到的候选词汇添加到结果列表
    }

    return predictions;
}
```

**解释：**

- 该方法首先检查数据库是否已经打开。如果未打开，则输出错误信息并返回空的预测结果。
- 使用 SQL 查询来选择匹配用户输入前缀的词汇。查询结果按优先级降序和词汇字典顺序升序排序，最多返回 4 个结果。
- 通过 `query.bindValue(":prefix", input + "%")` 将输入的前缀作为查询参数，执行查询后，将查询结果（候选词汇）添加到 `predictions` 向量中返回。

### 3.4 用户界面核心功能

**1.设置窗口属性 (置顶和防止抢夺焦点)**

```
KMainWindow::KMainWindow(QWidget *parent)
    : QMainWindow(parent), settingsDialog(new SettingsDialog(this))
{
    ui.setupUi(this);
    // 设置窗口为工具窗口、保持在最前且不抢夺焦点
    setWindowFlags(windowFlags()
        | Qt::WindowStaysOnTopHint);  // 保持窗口置顶

    // 使用 Windows API 设置扩展样式
    HWND hwnd = (HWND)this->winId();  // 获取窗口句柄
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE); // 获取当前扩展样式
    if (exStyle != 0) {
        exStyle |= 0x8000000; // 添加 WS_EX_NOACTIVATE 样式
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle); // 更新扩展样式
    }

```

**解释：**

- **`setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint)`**: 通过此代码行设置窗口始终位于所有其他窗口之上。`Qt::WindowStaysOnTopHint` 标志让窗口保持在最上层，无论用户切换到其他程序，这个窗口都不会被遮挡。这样做通常用于类似输入法或工具窗口，使其保持可见。
- **Windows API 设置扩展样式 (`WS_EX_NOACTIVATE`)**: 通过 Windows API 设置窗口的扩展样式。`WS_EX_NOACTIVATE` 样式使窗口不抢夺焦点，即使窗口位于最前面，它仍然不会干扰用户在其他程序中的输入焦点。`winId()` 获取了窗口的系统句柄，从而可以用 Windows API 函数修改该窗口的行为。
  - **`GetWindowLongPtr(hwnd, GWL_EXSTYLE)`**: 获取当前窗口的扩展样式。
  - **`SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle)`**: 更新扩展样式，添加 `WS_EX_NOACTIVATE`，确保窗口不抢夺焦点。

这种设置使得窗口可以保持在最前，但不会干扰用户对其他应用程序的操作（即不强行激活窗口或阻止用户在其他窗口中输入）。

**2.按钮点击输入的处理逻辑**

```
void KMainWindow::clickCandidateButtonsInput() { 
    // 获取发出信号的按钮
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        // 获取按钮的文字，作为输入内容
        QString input = button->text();
        if (input == "") return;  // 如果按钮的文字为空，则不做任何操作

        HWND hwndFocused = GetForegroundWindow();  // 获取当前获得焦点的窗口
        std::wstring wstr = input.toStdWString();  // 将按钮文本转换为 wstring 格式

        // 调用 SendKeys 函数发送输入内容
        SendKeys(hwndFocused, wstr);
    }
}
```

**解释：**

- **`qobject_cast<QPushButton\*>(sender())`**: 使用 `sender()` 获取发出信号的对象，这里是按钮。`qobject_cast<QPushButton*>` 将该对象转换为 `QPushButton` 类型，以便后续操作。
- **`QString input = button->text()`**: 获取按钮的文本内容，即用户点击的候选词。
- **`GetForegroundWindow()`**: 使用 Windows API 获取当前获得焦点的窗口句柄。这一步是为了知道用户正在操作哪个应用程序（例如文本编辑器、聊天窗口等），以便将输入发送到正确的位置。
- **`std::wstring wstr = input.toStdWString()`**: 将 `QString` 转换为 `std::wstring`，因为 `SendMessageW` 需要 `wstring` 类型的字符串。
- **`SendKeys(hwndFocused, wstr)`**: 调用 `SendKeys` 函数，将获取到的输入内容发送到当前获得焦点的窗口。这样，用户点击的按钮上的候选词就会被输入到当前窗口中。

**3.发送键盘消息到目标窗口**

```
void KMainWindow::SendKeys(HWND hwnd, const std::wstring& text) {
    for (wchar_t ch : text) {
        SendMessageW(hwnd, WM_CHAR, ch, 0);  // 使用 Windows API 发送字符
    }
}
```

**解释：**

- `SendMessageW(hwnd, WM_CHAR, ch, 0)`: 

  SendMessageW 是一个 Windows API 函数，用于向窗口发送消息。这里的消息类型是 WM_CHAR，表示一个字符的输入。

  - `hwnd`: 目标窗口的句柄，表示消息将发送到哪个窗口。
  - `WM_CHAR`: 表示字符输入的消息类型，Windows 系统会将该消息处理为文本输入。
  - `ch`: 要发送的字符，这里从 `text` 中逐个字符发送。
  - `0`: 额外的参数，这里不需要特殊处理，因此为 0。

通过这段代码，程序可以将用户选择的候选词模拟为键盘输入，发送到当前获得焦点的窗口。无论目标窗口是文本编辑器、浏览器还是聊天工具，用户点击输入法的候选词时，都能将该词输入到目标应用程序中。

## 4. 亮点介绍

- #### 1. **精准的手写识别功能**

  本输入法具备强大的手写识别功能，采用了 Microsoft Ink API 进行手写数据的捕捉和处理。用户只需在手写区域内进行书写，系统便能实时识别并转换为可编辑的文本，支持中文和非中文字符的识别，满足不同语言的输入需求。通过优化的笔迹流畅度和形态识别，本系统能够高效处理不同书写风格，确保识别结果的准确性和稳定性。

  #### 2. **智能的词组预测引擎**

  项目的核心亮点之一是智能的词组预测功能，基于 SQLite 存储的本地词库，系统能够根据用户的输入前缀精准预测可能的词组。结合上下文和输入历史，预测引擎不仅支持常见词组的优先显示，还能根据用户习惯调整词库中的词频，智能推荐最可能的候选词。该功能有效提升了输入效率，尤其是在长句输入时，能够极大减少用户的打字量。

  #### 3. **个性化的输入体验**

  通过灵活的设置功能，用户可以根据个人喜好定制输入法的行为和界面。例如，用户可以设置首选的候选词排序方式、手写输入区域的大小、手写笔迹的样式等。此外，支持中文与非中文的混合输入，使得用户在输入时能够无缝切换语言，不受限制地输入中英文混合文本。

## 5. 安装与部署

### 5.1 安装步骤

- 本项目的编译过程是采用课程提供的thirdparty_install工具包，在保证对应的文件结构并在对应的代码存放区域给出了对应CMakeLists.txt文档进行编译的，直接使用了start.bat脚本进行编译，具体的文件夹结构如下所示（只显示了存放该项目的结构，其他文件省略）

  ```
  ├── thirdparty_install
  ├── start.bat
  └── repository/
      ├── CMakeLists.txt
      └── LessonCode/
      	├── CMakeLists.txt
      	└── week0x/
      	    ├── CMakeLists.txt
      		└── homework/
      		    ├── Readme.md
      			└── SmartHandWriting/
      			    ├── CMakeLists.txt
      				└── 代码文件.cpp/.h
  
  ```


## 6. 存在问题与不足

由于个人经验不足导致的许多部分的设计模式并不符合规范，可能出现了很多冗余代码和类或函数之间强耦合的关系等。可能会影响对于项目的维护与更新

由于对于TSF涉及到的Windows底层API以及一些相关的编程知识缺少一定的了解导致无法使用TSF框架进行真正的输入法程序的编写，转而使用模拟键盘的方式进行实现，实际上只能与某些应用程序进行通讯，例如微信，QQ，浏览器，Typora等程序。而txt，word，Notepad++等文本编辑器以及ppt等应用程序则无法使用该输入法进行内容的输入。

由于打包程序的机制，导致数据库打包进去后只有在单次运行时能够正确更新数据，而一旦重新运行程序则会重置数据库中的相应数据，使得对于用户预测的记录功能只有在编译运行时才能够实现，直接运行打包好的exe文件无法实现。

不够了解前端的一些知识导致无法做出适应性强大的ui，勉强以固定大小的界面进行代替，且对于长文字的显示会出现按钮无法完全显示文字的情况。

对于应用程序切换焦点的一些相关内容程序中仍存在一些隐患。

识别的准确性依赖于使用的系统库而无法做进一步改进，实际使用上可能出现识别结果与所写内容大相径庭的情况。

数据库是从网络中搜索到的词库导入形成的，因此预测的词汇实际满足用户所需的概率较小。

## 7. 未来展望与改进

#### 7.1 架构优化

- 针对当前代码中存在的设计模式不规范、冗余代码以及类或函数间强耦合等问题，后续（如果可能的话）将进行全面的代码重构。引入设计模式原则，如单一职责原则、开闭原则等（前提是要经过认真的学习与实践），重新梳理类与函数的职责，降低耦合度，提高代码的可读性与可维护性。例如，将手写识别、文本预测等功能模块进一步抽象成独立的服务层，通过接口进行交互，使得各模块能够独立演进，减少相互影响。

#### 7.2 功能提升

- **手写识别优化**：鉴于当前识别准确性依赖系统库且无法进一步改进的问题，考虑学习引入深度学习技术。可能能够收集大量不同书写风格、字体的手写样本，训练自定义的手写识别模型，以提高对各类手写输入的识别准确率，减少识别结果与用户书写内容偏差较大的情况出现。

#### 7.3 用户体验改进

- **UI 优化**：学习前端知识，重新设计用户界面，使其具备自适应能力，能够根据不同设备屏幕尺寸动态调整布局。优化候选词显示方式，避免出现长文字导致按钮无法完全显示的问题，确保用户在各种设备上都能获得舒适的输入体验。

- **交互优化**：完善应用程序切换焦点相关逻辑，消除潜在隐患。例如，当用户在不同应用间切换时，输入法能够迅速响应，准确获取焦点并提供输入服务，避免出现输入卡顿或错误定位等问题。同时，进一步优化手写输入的流畅度，减少笔迹绘制延迟，提升实时反馈效果。

#### 7.4 数据管理完善

- 针对打包程序后数据库数据无法持久保存的问题，研究打包机制与数据库存储原理。采用合适的数据库持久化方案，如将数据库文件存储在独立的、可读写的用户目录下，确保无论通过编译运行还是直接执行 exe 文件，用户的输入历史、词库优先级等数据都能得到有效保存与更新，为个性化输入提供稳定的数据支持。

## 9. 总结

​	本次智能手写输入法的编写是我初次使用Qt框架构建一个相对完整且规模不算小的项目，从整体框架的设计到一个个类的完善，在通过各种信号与槽将各种类交织在一起，由于缺少经验导致设计模式不够良好，在编写过程写了许多冗余代码，同时在多次使用信号与槽有时还涉及到了信号的触发顺序等以前从未想到过的问题。在尝试了解TSF并实际使用时耗费了我大量的时间都没有真正的了解其整体的框架以及该如何使用，只能退而求其次的使用模拟键盘作为最后的方案，同时和输入密切相关的焦点问题也困扰了我许久，实在是缺乏对于某些系统库的相关知识，最后与同学讨论后才解决了这个问题。对于ui的设计也只能够参考其他输入法的页面，但无法做到能够适应不同的情况，最后实现出的ui也算差强人意。随着整体开发流程的结束，随之而来的就是各种bug以及类之间的交互问题，有时候遇到某些情况根本不知道该如何解决，或定位不到问题的所在。也算是在最后完成了这一个项目。虽然过程磕磕绊绊，但也确实让我学到了许多东西，即使不能够通过一个好的结构进行展示，但也会让我在下一次使用良好的结构编写程序时，对于其他的部分更加得心应手。同时我也深刻意识到一些涉及到操作系统等计算机底层知识的编程方式以及整体的排查问题解决问题的能力都是我缺少的，只能说要学和要练习的内容还有很多，希望自己的代码水平能够越来越精进，有一天也能构建出好的项目。
# HandWriting
