#ifndef CPREDICTION_H
#define CPREDICTION_H

class CPrediction
{
public:
    CPrediction(int label = 0,double confidence = 0);
    bool check_confidence(int threshold);
    int get_label(void);
    double get_confidence(void);
    void set_confidence(double confidence);
    void set_label(int label);
private:
    int m_label;
    double m_confidence;
};

#endif // CPREDICTION_H
