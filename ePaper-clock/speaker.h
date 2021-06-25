void testSpeaker()
{
#if defined(_HAS_SPEAKER_)
#ifdef _HAS_PWR_CTRL_
    pinMode(SPK_POWER_EN, OUTPUT);
    digitalWrite(SPK_POWER_EN, HIGH);
#endif
    ledcSetup(LEDC_CHANNEL_0, 1000, 8);
    ledcAttachPin(SPERKER_PIN, LEDC_CHANNEL_0);
    int i = 3;
    while (i--) {
        ledcWriteTone(LEDC_CHANNEL_0, 1000);
        delay(200);
        ledcWriteTone(LEDC_CHANNEL_0, 0);
    }
#ifdef _HAS_PWR_CTRL_
    pinMode(SPK_POWER_EN, INPUT);
#endif
    ledcDetachPin(SPERKER_PIN);
#endif
}
