#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QProgressBar>
#include "core/core.h"

namespace ui {

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    MainWindow(core::Core * core, QWidget * parent = nullptr);
private:
    void createWidgets();

    void fillSourceCombobox();

    void updateFilterState();

    Q_SLOT void onStartButtonClicked();
    Q_SLOT void onBrowseButtonClicked();

    Q_SLOT void onAudioSourceStarted();
    Q_SLOT void onAudioSourceStopped();
    Q_SLOT void onAudioSourceFailed();
    Q_SLOT void onPipelineCreationFailed();
    Q_SLOT void onAuthorizationFinished(bool success);
    Q_SLOT void onAudioCustomFilenameChanged();

    Q_SLOT void onFilterButtonClicked();
    Q_SLOT void onSourceComboboxIndexChanged();

    Q_SLOT void onSourceTimestampTimer();

    Q_SLOT void onSlowProcessingDetected();

private:
    QLabel* _sourceLabel = nullptr;
    QComboBox* _sourceCombobox = nullptr;

    QLabel* _useHeadphonesLabel = nullptr;

    QLabel* _filterLabel = nullptr;
    QPushButton* _filterButton = nullptr;

    QPushButton* _startButton = nullptr;

    QWidget* _browseFileWidget = nullptr;
    QLabel* _fileNameLabel = nullptr;
    QPushButton* _browseButton = nullptr;

    QTimer* _sourceTimestampTimer = nullptr;

    core::Core * _core;
};

} // namespace ui

#endif // UI_MAIN_WINDOW_H
