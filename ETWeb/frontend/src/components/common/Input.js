import {FormFeedback, FormGroup, Input, Label} from "reactstrap";
import React from "react";
import PropTypes from "prop-types";


export default class TextInput extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            name: props.name,
            labelText: props.labelText,
            type: props.type,
            required: props.required
        }
    }

    render() {
        const { name, labelText, type, required } = this.state;
        return (
            <FormGroup>
                <Label for={name}>{labelText}</Label>
                <Input invalid={this.props.error !== undefined}
                       autoComplete={type === 'password' ? "new-password" : ''}
                       onChange={this.props.onInputChange}
                       type={type}
                       name={name}
                       id={`#${name}`}
                       placeholder={required ? "*" : ""}/>
                <FormFeedback>{this.props.error}</FormFeedback>
            </FormGroup>
        );
    }
}

TextInput.propTypes = {
    name: PropTypes.string.isRequired,
    labelText: PropTypes.string.isRequired,
    onInputChange: PropTypes.func.isRequired,
    type: PropTypes.string,
    required: PropTypes.bool,
    error: PropTypes.string
};

TextInput.defaultProps = {
    type: "text",
    required: true,
    error: undefined,
};