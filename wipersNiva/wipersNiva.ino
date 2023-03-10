#define DEBUG 1
class GLWipers {
  public:
    void init(int wipersInPin, int washerInPin, int wipersRelayOutPin, int washerRelayOutPin, int maxWashingTime, int secondModePause, int thirdModePause) {
      _secondModePause = secondModePause;
      _thirdModePause = thirdModePause;
      _wipersInPin = wipersInPin;                                                                                                                               
      _washerInPin = washerInPin;
      _wipersRelayOutPin = wipersRelayOutPin;
      _washerRelayOutPin = washerRelayOutPin;
      _maxWashingTime = maxWashingTime;
      pinMode(_washerInPin, INPUT_PULLUP);
      pinMode(_wipersInPin, INPUT_PULLUP);
      pinMode(_wipersRelayOutPin, OUTPUT);
      pinMode(_washerRelayOutPin, OUTPUT);
      _lastMode = getSelectedMode();
      _wipersPulseSizeMillis = 1000;
    }

    void loop() {
      if (isStateChangedAndSetWipersMode()) {
#ifdef DEBUG
        Serial.print("_currentMode - ");
        Serial.println(_currentMode);
#endif
        if (_isWipersEnabled) {

          if (_currentMode != WIPERS_CONTINUOUS_MODE) {

            setTaskForOneWipe();
            setTimerForNextWipes();
          } else {
            stopTimerForNextWipes();
            continuousWipingStart();
          }
        } else {
          stopTimerForNextWipes();
          stopWiping();
        }

      }

      //continuous wiper task check
      multipleWipesLoop();
      oneWipeLoop();
    }


    void setTimerForNextWipes() {
      _nextWipesStartTimerTask = 1;
    }
    void stopTimerForNextWipes() {
      _nextWipesStartTimerTask = 0;
      _executeNextWipe = 0;
    }

    void setTaskForOneWipe() {
      _oneWipeStartTask = true;
    }
    void continuousWipingStart() {
      wipersEnablerDisabler(true);
    }
    void stopWiping() {
      wipersEnablerDisabler(false);
    }




  private:
    enum Wiper_modes
    {
      WIPERS_STOP_MODE = 1,
      WIPERS_FIRST_MODE = 2,
      WIPERS_SECOND_MODE = 3,
      WIPERS_CONTINUOUS_MODE = 4
    };
    unsigned long _timeOfOneWipeStarted = 0;
    int _maxWashingTime = 0;
    int _washerRelayOutPin = 0;
    int _wipersRelayOutPin = 0;
    int _wipersInPin = 0;
    int _washerInPin = 0;
    int _currentMode = 0;
    int _lastMode = 0;
    int _wipersPulseSizeMillis = 0;

    boolean _isWipersEnabled = 0;
    boolean _wipersTask = 0;
    boolean _washerTask = 0;
    boolean _oneWipeStartTask = 0;
    boolean _oneWipeStopTask = 0;
    boolean _nextWipesStartTimerTask = 0;
    boolean _executeNextWipe = 0;
    unsigned long _timeOfMultipleWipesTimerStarted = 0;


    boolean _commandToEnableWasher = 0;;

    int _currentPause = 0;
    int _secondModePause = 0;
    int _thirdModePause = 0;

    boolean isTimePassed(unsigned long startedAt, int timeInterval) {
      boolean passed = 0;
      if ((millis() - startedAt) >= timeInterval) {
        passed = 1;
      }
      return passed;
    }
    void multipleWipesLoop() {
      if (_nextWipesStartTimerTask) {//?????????????????? ????????????
        _timeOfMultipleWipesTimerStarted = millis();
        _nextWipesStartTimerTask = 0;
        _executeNextWipe = 1;
      }
      if (_executeNextWipe) {//?????????????????? 1 ?????????? ???? ?????????????????? ??????????????
        if (isTimePassed(_timeOfMultipleWipesTimerStarted, _currentPause)) {
          setTaskForOneWipe();
          _nextWipesStartTimerTask = 1;
        }
      }
    }

    void oneWipeLoop() {
      if (_oneWipeStartTask) {
        wipersEnablerDisabler(true);
        _timeOfOneWipeStarted = millis();
        _oneWipeStartTask = 0;
        _oneWipeStopTask = 1;
      }
      if (_oneWipeStopTask) {
        if (!_oneWipeStartTask && isTimePassed(_timeOfOneWipeStarted, _wipersPulseSizeMillis)) {
          wipersEnablerDisabler(false);
          _oneWipeStopTask = 0;
        }
      }
    }

    boolean isStateChangedAndSetWipersMode() {
      boolean changed = 0;
      int wipersMode =  getSelectedMode();

      if (_lastMode != mode) {

        wipersSetMode(mode);
        _lastMode = wipersMode;
        changed = 1;
      } else {
        changed = 0;
      }
      return changed;
    }

    int getSelectedMode() {
      int selectorMode = analogRead(_wipersInPin);
      int wipersMode = 1;
      if (selectorMode > 1000) {
        wipersMode = 1;
        return wipersMode;
      }
      if (selectorMode > 25 && selectorMode <= 40) {
        wipersMode = 2;
        return wipersMode;
      }
      if (selectorMode > 20 && selectorMode <= 25) {
        wipersMode = 3;
        return wipersMode;
      }
      if (selectorMode > 10 && selectorMode <= 20) {
        wipersMode = 4;
        return wipersMode;
      }
      return wipersMode;
    }

    void wipersEnablerDisabler(boolean enable) {
      if (!enable) {
        digitalWrite(_wipersRelayOutPin, LOW);
      }
      if (enable) {
        digitalWrite(_wipersRelayOutPin, HIGH);
      }
    }

    void wipersSetMode(int mode) {
      _currentMode = wipersMode;
      switch (mode) {
        case WIPERS_STOP_MODE:
          _isWipersEnabled = 0;
          break;
        case WIPERS_FIRST_MODE:
          _isWipersEnabled = 1;
          _currentPause = _secondModePause;
          break;
        case WIPERS_SECOND_MODE:
          _isWipersEnabled = 1;
          _currentPause = _thirdModePause;
          break;
        case WIPERS_CONTINUOUS_MODE:
          _isWipersEnabled = 1;
          _currentPause = 0;
          break;
      }
    }

};
