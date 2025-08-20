#include "main_window.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QCoreApplication>

namespace ui {

MainWindow::MainWindow(core::Core * core, QWidget *parent)
    : QMainWindow(parent)
    , _core(core)
{
    setMinimumSize(QSize(300, 400));
    setWindowTitle("Audio Effects SDK Demo Application");
    resize(400, 400);

    createWidgets();
    updateFilterState();

    connect(_core,
        &core::Core::audioSourceStarted,
        this,
        &MainWindow::onAudioSourceStarted
    );

    connect(_core,
        &core::Core::audioSourceStopped,
        this,
        &MainWindow::onAudioSourceStopped
    );

    connect(_core,
        &core::Core::audioSourceFailed,
        this,
        &MainWindow::onAudioSourceFailed
    );

    connect(_core,
        &core::Core::audioPipelineCreationFailed,
        this,
        &MainWindow::onPipelineCreationFailed
    );

    connect(_core,
        &core::Core::authorizationFinished,
        this,
        &MainWindow::onAuthorizationFinished
    );

    connect(_core,
        &core::Core::audioCustomFilenameChanged,
        this,
        &MainWindow::onAudioCustomFilenameChanged
    );
    connect(_core,
        &core::Core::audioCustomFilenameChanged,
        this,
        &MainWindow::onAudioCustomFilenameChanged
    );

    connect(_core->sdkNotificationHandler(),
        &core::SDKNotificationHandler::slowProcessingDetected,
        this,
        &MainWindow::onSlowProcessingDetected
    );

    _sourceTimestampTimer = new QTimer(this);
    connect(_sourceTimestampTimer, &QTimer::timeout,
        this, &MainWindow::onSourceTimestampTimer);
    _sourceTimestampTimer->start(200);

    _core->startAuth();
}

void MainWindow::createWidgets()
{
    QWidget * centralWidget = new QWidget;
    this->setCentralWidget(centralWidget);

    _sourceLabel = new QLabel(tr("Source:"));

    QHBoxLayout * sourceLayout = new QHBoxLayout();
    sourceLayout->setContentsMargins(0, 0, 0, 0);
    sourceLayout->addWidget(_sourceLabel, 1, Qt::AlignLeft);

    _sourceCombobox = new QComboBox;
    fillSourceCombobox();
    connect(_sourceCombobox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &MainWindow::onSourceComboboxIndexChanged);

    _useHeadphonesLabel = new QLabel("Use headphones to avoid echo!");
    _useHeadphonesLabel->setStyleSheet("QLabel { color : #c9bd0a; }");
    _useHeadphonesLabel->hide();

    _filterLabel = new QLabel;

    _filterButton = new QPushButton;
    connect(
        _filterButton, &QPushButton::clicked,
        this, &MainWindow::onFilterButtonClicked,
        Qt::QueuedConnection
    );

    QHBoxLayout * filterLayout = new QHBoxLayout();
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->addWidget(_filterLabel, 0, Qt::AlignLeft);

    _browseFileWidget = new QWidget;
    _fileNameLabel = new QLabel(tr("File:"));
    _browseButton = new QPushButton(tr("Browse..."));
    QHBoxLayout * browseLayout = new QHBoxLayout();
    browseLayout->setContentsMargins(0,0,0,0);
    _browseFileWidget->setLayout(browseLayout);
    browseLayout->addWidget(_fileNameLabel, 1, Qt::AlignLeft);
    browseLayout->addWidget(_browseButton, 0, Qt::AlignRight);

    connect(_browseButton,
        &QAbstractButton::clicked,
        this,
        &MainWindow::onBrowseButtonClicked
    );

    _startButton = new QPushButton(tr("Start"));
    _startButton->setDisabled(true);

    QVBoxLayout * mainLayout = new QVBoxLayout();

    mainLayout->addLayout(sourceLayout);
    mainLayout->addWidget(_sourceCombobox);
    mainLayout->addWidget(_browseFileWidget);
    mainLayout->addWidget(_useHeadphonesLabel);
    mainLayout->addSpacing(10);

    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(_filterButton);

    mainLayout->addStretch();
    mainLayout->addWidget(_startButton);

    connect(_startButton,
        &QAbstractButton::clicked,
        this,
        &MainWindow::onStartButtonClicked
    );

    onSourceComboboxIndexChanged();

    this->centralWidget()->setLayout(mainLayout);
}

void MainWindow::fillSourceCombobox()
{
    _sourceCombobox->clear();

    _sourceCombobox->addItem(
        QString("Noise recording #1 (Air conditioning)"),
        QString(":/noise1_air_conditioning.wav"));

    _sourceCombobox->addItem(
        QString("Noise recording #2 (Bus)"),
        QString(":/noise2_bus.wav"));

    for (const QString & micName : _core->allMicNames()) {
        _sourceCombobox->addItem(micName, "--mic:" + micName);
    }
    _sourceCombobox->addItem(QString("Custom Audio File"), QString("--custom-file"));
}

void MainWindow::updateFilterState()
{
    bool isEnabled = _core->isNoiseSuppressionEnabled();
    _filterButton->setText(isEnabled?
        tr("Disable") : tr("Enable")
    );
    _filterLabel->setText(isEnabled ? 
        tr("Noise Suppression: Enabled") : tr("Noise Suppression: Disabled")
    );
}

void MainWindow::onStartButtonClicked()
{
    QString sourceName = _sourceCombobox->itemData(_sourceCombobox->currentIndex()).toString();
    if (sourceName.startsWith("--mic:")) {
        QString micName = _sourceCombobox->itemText(_sourceCombobox->currentIndex());
        _core->startAudioSource(micName, core::AudioSourceType::Microphone);
    } else if (sourceName == "--custom-file") {
        _core->startAudioSource(_core->customAudioFilename(), core::AudioSourceType::File);
    } else {
        _core->startAudioSource(sourceName, core::AudioSourceType::File);
    }

    onSourceTimestampTimer();
}

void MainWindow::onBrowseButtonClicked()
{
    QString curFilename = _core->customAudioFilename();
    if (curFilename.isEmpty()) {
        curFilename = qApp->applicationDirPath();
    }

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.selectFile(curFilename);
    dialog.setNameFilters({
        "Audio files (*.wav)",
        "All files (*.*)"
    }); 

    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.isEmpty() == false) {
            _core->setCustomAudioFilename(fileNames.first());
        }
    }
}

void MainWindow::onAudioSourceStarted()
{
    qDebug() << "Audio source started";
}

void MainWindow::onAudioSourceStopped()
{
    qDebug() << "Audio source stopped";
}

void MainWindow::onAudioSourceFailed()
{
    QMessageBox msgBox;
    msgBox.setText("Audio Source Failed");
    msgBox.exec();
}

void MainWindow::onPipelineCreationFailed()
{
    QMessageBox msgBox;
    msgBox.setText("SDK Pipeline Creation Failed");
    msgBox.exec();
}

Q_SLOT void MainWindow::onAuthorizationFinished(bool success)
{
    if (success) {
        _startButton->setEnabled(true);
        return;
    }

    QMessageBox::critical(
        this, 
        "Authorization Failed", 
        "Check your internet connection and validate your customer token."
    );
}

void MainWindow::onAudioCustomFilenameChanged()
{
    QFileInfo fi(_core->customAudioFilename());
    _fileNameLabel->setText("File: " + fi.fileName());
}

Q_SLOT void MainWindow::onFilterButtonClicked()
{
    bool isEnabled = _core->isNoiseSuppressionEnabled();
    _core->setNoiseSuppressionEnabled(!isEnabled);
    updateFilterState();
}

void MainWindow::onSourceComboboxIndexChanged()
{
    QString sourceName = _sourceCombobox->itemData(_sourceCombobox->currentIndex()).toString();
    if (sourceName.startsWith("--mic:")) { // mic
        _useHeadphonesLabel->show();
    } else {
        _useHeadphonesLabel->hide();
    }

    if (sourceName == "--custom-file") {
        _browseFileWidget->show();
    } else {
        _browseFileWidget->hide();
    }
}

void MainWindow::onSourceTimestampTimer()
{
    if (_core->hasAudioSource()) {
        double cur = _core->playbackProcessedSec();
        int dur = _core->audioSourceDurationSec();
        QString durStr = dur != -1 ? QString::number(dur) : "--";
        _sourceLabel->setText(tr("Source:") 
            + QString(" %1/%2").arg(QString::number(cur, 'g', 2), durStr));
    } else {
        _sourceLabel->setText(tr("Source:"));
    }

}

Q_SLOT void MainWindow::onSlowProcessingDetected()
{
    QMessageBox::warning(
        nullptr, 
        "Slow Audio Processing Detected!", 
        "Turn off noise suppression if the sound is distorted."
    );
}

} // namespace ui